/*
 * dialer.c
 *
 *  Created on: Jun 10, 2015
 *      Author: jcobb
 */

#include <stdint.h>
#include <string.h>


/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"


#include "freertos_usart_serial.h"

#include "sysclk.h"
#include "modem_defs.h"
#include "dialer.h"
#include "ppp.h"

static volatile bool cmd_dial = false;


#define BUFFER_MAX		50
#define BUFFER_SIZE     (BUFFER_MAX + 1)
#define QUEUE_TICKS		0
static signed char outBuffer[BUFFER_SIZE];
QueueHandle_t xDialerQueue = NULL;

xSemaphoreHandle dial_signal = 0;

/*
const at_command_t at_commands[] =
{
		{MODEM_CMD_AT, 			MODEM_TOKEN_OK, 3, 3},
		{MODEM_CMD_ECHOOFF, 	MODEM_TOKEN_OK, 3, 3},
		{MODEM_CMD_CFUN, 		MODEM_TOKEN_OK, 3, 3},
		{MODEM_CMD_CREG, 		MODEM_TOKEN_OK, 3, 3},
		{MODEM_CMD_GAUTH, 		MODEM_TOKEN_OK, 3, 3},
		{MODEM_CMD_SETCONTEXT, 	MODEM_TOKEN_OK, 3, 3},
		{MODEM_CMD_DIAL, 		MODEM_TOKEN_CONNECT, 3, 3},
		{NULL, NULL, 0, 0}
};
*/

const at_command_t at_commands[] =
{
		{"AT\r\n", 				MODEM_TOKEN_OK, 3, 3},
		{"ATE0\r\n", 			MODEM_TOKEN_OK, 3, 3},
		{"AT+CFUN=1\r\n", 		MODEM_TOKEN_OK, 3, 3},
		{"AT+CREG=1\r\n", 		MODEM_TOKEN_OK, 3, 3},
		{"AT#GAUTH=0\r\n", 		MODEM_TOKEN_OK, 3, 3},
		{"AT+CGDCONT=1,\"IP\",\"c1.korem2m.com\"\r\n", 	MODEM_TOKEN_OK, 3, 3},
		{"ATD*99***1#\r\n", 	MODEM_TOKEN_CONNECT, 3, 3},
		{NULL, NULL, 0, 0}
};

/* Const strings used by the CLI interface. */
static const uint8_t *const dialer_message = (uint8_t *) "ATSAM3X8E Initializing Modem...\r\n";

static uint8_t * dialer_cmd_at_at = (uint8_t*) MODEM_CMD_AT;
static uint8_t * dialer_cmd_at_echooff = (uint8_t*) MODEM_CMD_ECHOOFF;
static uint8_t * dialer_cmd_at_cfun = (uint8_t*) "AT+CFUN=1\r\n";
static uint8_t * dialer_cmd_at_creg = (uint8_t*) "AT+CREG=1\r\n";
static uint8_t * dialer_cmd_at_auth = (uint8_t*) "AT#GAUTH=0\r\n";
static uint8_t * dialer_cmd_at_apn = (uint8_t*) "AT+CGDCONT=1,\"IP\",\"c1.korem2m.com\"\r\n";
static uint8_t * dialer_cmd_at_dial = (uint8_t*) "ATD*99***1#\r\n";


typedef enum
{
	SYS_ERR_OK = 0,
	SYS_ERR_AT_FAIL,
	SYS_ERR_FAIL_OTHERS
}sys_err;


/* The buffer provided to the USART driver to store incoming character in. */
static uint8_t receive_buffer[RX_BUFFER_SIZE_BYTES] = {0};

/* The USART instance used for input and output. */
static freertos_usart_if ppp_usart = NULL;

static void ppp_dialer_task(void *pvParameters);
static sys_err wait_return(char * token, char ** ptr_out, u8_t seconds);

static void init_ppp(void);
static uint8_t modem_init(void);

static void SEND_AT(uint8_t *cmd);


static uint8_t modem_init(void)
{

	const at_command_t* at_cmd = &(at_commands[0]);
	//SEND_AT((uint8_t*)at_cmd->cmd);

	while(at_cmd->cmd != NULL)
	{
		SEND_AT((uint8_t*)at_cmd->cmd);
		at_cmd++;

//		char * ptr = NULL;
//
//		if(wait_return(at_cmd->result, &ptr,  at_cmd->timeout) == SYS_ERR_OK)
//			at_cmd++;
//		else
//			return SYS_ERR_AT_FAIL;

//		for(u8_t r_time=0; r_time<cmd->retries; r_time++) {
//			SEND_AT(cmd->cmd);
//			if(wait_return(cmd->result, cmd->timeout) == SYS_ERR_OK) {
//				break;
//			}
//		}
	}

	return SYS_ERR_OK;

//	SEND_AT(dialer_cmd_at_echooff);
//
//	SEND_AT(dialer_cmd_at_cfun);
//	SEND_AT(dialer_cmd_at_creg);
//	SEND_AT(dialer_cmd_at_auth);
//	SEND_AT(dialer_cmd_at_apn);
//	SEND_AT(dialer_cmd_at_dial);
}

volatile u32_t _sys_jiffies_ = 0;

void sys_init(void)
{
	_sys_jiffies_ = sys_now();
}

u32_t sys_now(void)
{
	return (u32_t) xTaskGetTickCount();
}

u32_t sio_write(sio_fd_t fd, u8_t *data, u32_t len)
{
	return 0;
}


u32_t sys_jiffies(void)
{
	return _sys_jiffies_;
}

void modem_error_cb(void *ctx, int errCode, void *arg) {

}

static void SEND_AT(uint8_t *cmd)
{
	uint8_t *output_string;

	portTickType max_block_time_ticks = 200UL / portTICK_RATE_MS;

	static char * tmp_buffer[400];

	output_string = (uint8_t *) tmp_buffer;

	strcpy((char *) output_string, (char *) cmd);
	freertos_usart_write_packet(ppp_usart, output_string,
								strlen((char *) cmd),
								max_block_time_ticks);
//
//	freertos_usart_write_packet(ppp_usart, cmd,
//								strlen((char*)cmd),
//								max_block_time_ticks);
}
#define 	MAX_INPUT_SIZE		50

static sys_err wait_return(char * token, char ** ptr_out, u8_t seconds)
{
	char * ptr = NULL;

	//char * tmp_str = token;
	u32_t time = sys_now();

	//u8_t ch;
	uint8_t received_char;
	uint8_t input_index = 0;
	static int8_t input_string[MAX_INPUT_SIZE];

	portTickType max_block_time_ticks = 200UL / portTICK_RATE_MS;

	input_index = 0;
	memset(input_string, '\0', MAX_INPUT_SIZE);

	// convert to seconds
	u32_t timeout = (seconds*1000);

	//TODO: revise
	while((sys_now() - time) < timeout)
	{

		if (freertos_usart_serial_read_packet(ppp_usart, &received_char, sizeof(received_char), portMAX_DELAY) == sizeof(received_char)) {

			if((ptr = strstr(input_string, token))) {
				if(ptr_out != NULL) {
					*ptr_out = ptr;
					return SYS_ERR_OK;
				}
			} else if (input_index < MAX_INPUT_SIZE) {
				input_string[input_index] = received_char;
				input_index++;
			}

		}
	}

	return SYS_ERR_AT_FAIL;
}



void create_dialer_usart_task(Usart *usart_base, uint16_t stack_depth_words, unsigned portBASE_TYPE task_priority)
{
	vSemaphoreCreateBinary(dial_signal);

	freertos_usart_if freertos_usart;
	freertos_peripheral_options_t driver_options = {
		receive_buffer,									/* The buffer used internally by the USART driver to store incoming characters. */
		RX_BUFFER_SIZE_BYTES,							/* The size of the buffer provided to the USART driver to store incoming characters. */
		configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY,	/* The priority used by the USART interrupts. */
		USART_RS232,									/* Configure the USART for RS232 operation. */
		(WAIT_TX_COMPLETE | USE_TX_ACCESS_MUTEX)		/* Use access mutex on Tx (as more than one task transmits) but not Rx. Wait for a Tx to complete before returning from send functions. */
	};

	const sam_usart_opt_t usart_settings = {
		DIALER_BAUD_RATE,
		US_MR_CHRL_8_BIT,
		US_MR_PAR_NO,
		US_MR_NBSTOP_1_BIT,
		US_MR_CHMODE_NORMAL,
		0 /* Only used in IrDA mode. */
	};

	/* Initialise the USART interface. */
	freertos_usart = freertos_usart_serial_init(usart_base,
			&usart_settings,
			&driver_options);
	configASSERT(freertos_usart);


	sys_init();





	/* Create the USART CLI task. */
	xTaskCreate(	ppp_dialer_task,			/* The task that implements the command console. */
					(const int8_t *const) "PPP_D",	/* Text name assigned to the task.  This is just to assist debugging.  The kernel does not use this name itself. */
					stack_depth_words,					/* The size of the stack allocated to the task. */
					(void *) freertos_usart,			/* The parameter is used to pass the already configured USART port into the task. */
					task_priority,						/* The priority allocated to the task. */
					NULL);

}

static void ppp_dialer_task(void *pvParameters)
{
	uint8_t received_char, input_index = 0, *output_string;
	static int8_t input_string[MAX_INPUT_SIZE],
			last_input_string[MAX_INPUT_SIZE];
	portBASE_TYPE returned_value;
	portTickType max_block_time_ticks = 200UL / portTICK_RATE_MS;

	ppp_usart = (freertos_usart_if) pvParameters;
	configASSERT(ppp_usart);


	pppInit();
	// TODO: init string and dial the modem

	modem_init();

	pppSetAuth(PPPAUTHTYPE_NONE, "", "");

	void * link_status_ctx;

	int pppFd = pppOpen((sio_fd_t) ppp_usart, modem_error_cb, link_status_ctx);



	/* Obtain the address of the output buffer.  Note there is no mutual
	exclusion on this buffer as it is assumed only one command console
	interface will be used at any one time. */

	static char * tmp_buffer[400];

	output_string = (uint8_t *) tmp_buffer;

	/* Send the welcome message.  The message is copied into RAM first as the
	DMA cannot send from Flash addresses. */
	strcpy((char *) output_string, (char *) dialer_message);
	freertos_usart_write_packet(ppp_usart, output_string,
			strlen((char *) dialer_message),
			max_block_time_ticks);







	BaseType_t result;


	for(;;) {

		//result = xQueueReceive(xDialerQueue, outBuffer, QUEUE_TICKS);

		//if(result == pdTRUE) {
		if(xSemaphoreTake(dial_signal, portMAX_DELAY)) {

			strcpy((char *) output_string, (char *) dialer_cmd_at_dial);
			freertos_usart_write_packet(ppp_usart, output_string, strlen((char *) dialer_cmd_at_dial), max_block_time_ticks);
		}

//		if(cmd_dial) {
//			// toggle our flag
//			cmd_dial = false;
//			strcpy((char *) output_string, (char *) dialer_cmd_dial);
//			freertos_usart_write_packet(cli_usart, output_string, strlen((char *) dialer_cmd_dial), max_block_time_ticks);
//		}



		//vTaskDelay(200);
	}
}

extern void dial(void)
{
	cmd_dial = true;
}


