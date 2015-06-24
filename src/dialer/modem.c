/*
 * modem.c
 *
 *  Created on: Jun 16, 2015
 *      Author: jcobb
 */

#include <stdint.h>
#include <string.h>
#include "FreeRTOS.h"
#include "modem.h"
#include "modem_defs.h"
#include "freertos_usart_serial.h"
#include "board.h"
#include "sys_arch.h"


const at_command_t at_commands[] =
{
		{MODEM_CMD_AT, 			MODEM_TOKEN_OK, 3, 3},
		{MODEM_CMD_ECHOOFF, 	MODEM_TOKEN_OK, 3, 3},
		{MODEM_CMD_CFUN, 		MODEM_TOKEN_OK, 3, 3},
		{MODEM_CMD_CREG, 		MODEM_TOKEN_OK, 3, 3},
		{MODEM_CMD_GAUTH, 		MODEM_TOKEN_OK, 3, 3},
		//{MODEM_CMD_SETCONTEXT, 	MODEM_TOKEN_OK, 3, 3},
		{MODEM_CMD_DIAL, 		MODEM_TOKEN_CONNECT, 3, 3},
		{NULL, NULL, 0, 0}
};


// human readable version of at commands
// these commands include the additional linefeed so we
// can read the output via a terminal session
//const at_command_t at_commands[] =
//{
//		{"AT\r\n", 				MODEM_TOKEN_OK, 3, 3},
//		{"ATE0\r\n", 			MODEM_TOKEN_OK, 3, 3},
//		{"AT+CFUN=1\r\n", 		MODEM_TOKEN_OK, 3, 3},
//		{"AT+CREG=1\r\n", 		MODEM_TOKEN_OK, 3, 3},
//		{"AT#GAUTH=0\r\n", 		MODEM_TOKEN_OK, 3, 3},
//		{"AT+CGDCONT=1,\"IP\",\"c1.korem2m.com\"\r\n", 	MODEM_TOKEN_OK, 3, 3},
//		{"ATD*99***1#\r\n", 	MODEM_TOKEN_CONNECT, 3, 3},
//		{NULL, NULL, 0, 0}
//};



/* The buffer provided to the USART driver to store incoming character in. */
static uint8_t receive_buffer[RX_BUFFER_SIZE_BYTES] = {0};

static void init_hw(void);
static void init_usart(Usart *usart_base);
static void SEND_AT(uint8_t *cmd);
static sys_result handle_result(char * token, char ** ptr_out, uint8_t seconds);



//uint32_t pow_mon = pio_get_pin_value(MDM_POWMON_IDX);
//uint32_t pow_mon2 = pio_get(PIOC, PIO_TYPE_PIO_INPUT, PIO_PC25);

void init_hw(void)
{

	//printf("initializing modem hardware...\r\n");
	//SEND_AT("init_hw\r\n");
	// Sanity check make sure pins are low
	pio_set_pin_low(MDM_ENABLE_IDX);
	pio_set_pin_low(MDM_RESET_IDX);
	pio_set_pin_low(MDM_ONOFF_IDX);

	//printf("checking modem power monitor\r\n");

	// Chek for power on

//	uint8_t num_tries = 1;
//
//	while(true) {
//
//		printf("try %d\r\n", num_tries);
//		if(num_tries++ == 3) break;
//
//		//printf("delay 3 sec.\r\n");
//		vTaskDelay(3000);
//
//		uint32_t powmon = 0;
//		powmon = pio_get_pin_value(MDM_POWMON_IDX);
//		vTaskDelay(2);
//
//		printf("powmon=%d\r\n", powmon);
//
//		uint32_t foo = 0;
//		//if(pio_get_pin_value(MDM_POWMON_IDX) == 0) {
//		if(powmon == 0) {
//			break;
//		} else {
//			//printf("toggle modem enable high-low\r\n");
//			pio_set_pin_low(MDM_ENABLE_IDX);
//			vTaskDelay(10);
//			pio_set_pin_high(MDM_ENABLE_IDX);
//
//		}
//	}


	while(true) {
		pio_set_pin_high(MDM_ONOFF_IDX);
		vTaskDelay(3000);

		pio_set_pin_low(MDM_ONOFF_IDX);
		vTaskDelay(2000);
		if(pio_get_pin_value(MDM_POWMON_IDX) == 1)
			break;

	}



	printf("init_hw done\r\n");

}



uint8_t modem_init(void)
{
	// Hardware must be initialized before usart



	// init hardware pins first
	init_hw();

	// configure usart pins
	board_init_modem_usart();

	// init usart
	init_usart(MODEM_USART);

	const at_command_t* at_cmd = &(at_commands[0]);
	//SEND_AT((uint8_t*)at_cmd->cmd);

	while(at_cmd->cmd != NULL)
	{
		printf("SENDING %s\r\n", at_cmd->cmd);
		SEND_AT((uint8_t*)at_cmd->cmd);
		at_cmd++;

		char * ptr = NULL;

		sys_result sys_status = handle_result(at_cmd->result, &ptr,  at_cmd->timeout);

		if(sys_status == SYS_AT_OK) {
			//at_cmd++;
			printf("SYS_OK\r\n");
			continue;
		} else if (sys_status == SYS_ERR_AT_FAIL) {
			printf("SYS_ERR_AT_FAIL\r\n");
		} else if (sys_status == SYS_ERR_AT_NOCARRIER) {
			printf("SYS_ERR_AT_NO_CARRIER\r\n");
		} else if(sys_status == SYS_ERR_AT_TIMEOUT) {
			printf("SYS_ERR_AT_TIMEOUT\r\n");
			//return SYS_ERR_AT_FAIL;
		}

//		for(int r_time=0; r_time< at_cmd->retries; r_time++) {
//			SEND_AT(at_cmd->cmd);
//			if(handle_result(at_cmd->result, &ptr, at_cmd->timeout) == SYS_OK) {
//				printf("token found\r\n");
//			}
//		}
	}

	return SYS_OK;

//	SEND_AT(dialer_cmd_at_echooff);
//
//	SEND_AT(dialer_cmd_at_cfun);
//	SEND_AT(dialer_cmd_at_creg);
//	SEND_AT(dialer_cmd_at_auth);
//	SEND_AT(dialer_cmd_at_apn);
//	SEND_AT(dialer_cmd_at_dial);
}



static void init_usart(Usart *usart_base)
{

	freertos_peripheral_options_t driver_options = {
		receive_buffer,									/* The buffer used internally by the USART driver to store incoming characters. */
		RX_BUFFER_SIZE_BYTES,							/* The size of the buffer provided to the USART driver to store incoming characters. */
		configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY,	/* The priority used by the USART interrupts. */
		USART_RS232,									/* Configure the USART for RS232 operation. */
		//UART_RS232,
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
	modem_usart = freertos_usart_serial_init(usart_base,
			&usart_settings,
			&driver_options);
	configASSERT(modem_usart);
}


static void SEND_AT(uint8_t *cmd)
{
	uint8_t *output_string;

	portTickType max_block_time_ticks = 200UL / portTICK_RATE_MS;

	static char * tmp_buffer[400];

	output_string = (uint8_t *) tmp_buffer;

	strcpy((char *) output_string, (char *) cmd);

	freertos_usart_write_packet(modem_usart, output_string,
								strlen((char *) cmd),
								max_block_time_ticks);
}



static sys_result handle_result(char * token, char ** ptr_out, uint8_t seconds)
{
	printf("handle_result\r\n");
	char * ptr = NULL;

	//char * tmp_str = token;
	uint32_t time = sys_now();

	//printf("time: %d\r\n", time);




	//u8_t ch;
	uint8_t received_char;
	uint8_t input_index = 0;
	static int8_t input_string[MAX_INPUT_SIZE];

	portTickType max_block_time_ticks = 200UL / portTICK_RATE_MS;

	input_index = 0;
	memset(input_string, '\0', MAX_INPUT_SIZE);

	// convert to seconds
	uint32_t timeout = ((uint32_t)sys_now()) + (seconds*1000);

	//printf("t0:%d t1:%d\r\n", sys_now(), timeout);
	//TODO: revise
	while(sys_now() <= timeout)
	{
		//printf("t0:%d t1:%d\r\n", sys_now(), timeout);
		if (freertos_usart_serial_read_packet(modem_usart, &received_char, sizeof(received_char), portMAX_DELAY) == sizeof(received_char)) {

			//printf("received %c", (char) received_char);


			if((ptr = strstr(input_string, token))) {
				if(ptr_out != NULL) {
					*ptr_out = ptr;
					//printf("sys_ok\r\n");
					return SYS_AT_OK;
				}
			} else if ((ptr = strstr(input_string, MODEM_TOKEN_ERROR))) {
				*ptr_out = ptr;
				return SYS_ERR_AT_FAIL;
			} else if((ptr = strstr(input_string, MODEM_TOKEN_NOCARRIER))) {
				*ptr_out = ptr;
				return SYS_ERR_AT_NOCARRIER;
			} else if (input_index < MAX_INPUT_SIZE) {
				input_string[input_index] = received_char;
				printf("buffer: %s\r\n", input_string);
				input_index++;
			}

		}
	}
	//printf("sys_err_at_fail\r\n");
	return SYS_ERR_AT_TIMEOUT;
}


sys_result process_modem(char * token, char ** ptr_out, uint8_t seconds);
void init_modem_buffer(void);
sys_result read_modem_usart(uint8_t seconds);


uint8_t modem_lines[MAX_INPUT_SIZE+1];
uint8_t modem_line_index = 0;

uint8_t modem_buffer[MAX_INPUT_SIZE+1];
uint8_t modem_buffer_index = 0;

sys_result process_modem(char * token, char ** ptr_out, uint8_t seconds)
{
	if(read_modem_usart(seconds) == SYS_AT_OK) {

		char * ptr = NULL;

		if((ptr = strstr(modem_lines, token))) {
			if(ptr_out != NULL) {
				*ptr_out = ptr;
				//printf("sys_ok\r\n");
				return SYS_AT_OK;
			}
		} else if ((ptr = strstr(modem_lines, MODEM_TOKEN_ERROR))) {
			*ptr_out = ptr;
			return SYS_ERR_AT_FAIL;
		} else if((ptr = strstr(modem_lines, MODEM_TOKEN_NOCARRIER))) {
			*ptr_out = ptr;
			return SYS_ERR_AT_NOCARRIER;
		}
	}

	return SYS_ERR_AT_TIMEOUT;
}

sys_result read_modem_usart(uint8_t seconds)
{
	uint8_t c;
	uint8_t buffer_index = 0;

	// convert to seconds
	uint32_t timeout = ((uint32_t)sys_now()) + (seconds*1000);
	//portTickType max_block_time_ticks = 200UL / portTICK_RATE_MS;

	while(sys_now() <= timeout)
	{
		if (freertos_usart_serial_read_packet(modem_usart, &c, sizeof(c), portMAX_DELAY) == sizeof(c)) {

			if(c == '\0')
				continue;

			modem_buffer[buffer_index] = c;
			buffer_index++;

			if(buffer_index >= MAX_INPUT_SIZE)
				break;

			if (c == '\r' || c == '\n') {
				strcpy(modem_buffer, modem_lines);
				init_modem_buffer();
				return SYS_AT_OK;
			}
		}
	}

	return SYS_ERR_AT_TIMEOUT;
}

void init_modem_buffer(void)
{
	modem_line_index = 0;
	memset(modem_buffer, '\0', sizeof(modem_buffer));
}

void init_modem_lines(void)
{
	memset(modem_lines, '\0', sizeof(modem_lines));
}

