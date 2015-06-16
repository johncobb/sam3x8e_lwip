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
#include "cph_board.h"


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

// human readable version of at commands
// these commands include the additional linefeed so we
// can read the output via a terminal session
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



/* The buffer provided to the USART driver to store incoming character in. */
static uint8_t receive_buffer[RX_BUFFER_SIZE_BYTES] = {0};

static void init_hw(void);
static void init_usart(Usart *usart_base);
static void SEND_AT(uint8_t *cmd);
static sys_err handle_result(char * token, char ** ptr_out, uint8_t seconds);


void init_hw(void)
{

	// Sanity check make sure pins are low
	pio_set_pin_low(MDM_ENABLE_IDX);
	pio_set_pin_low(MDM_RESET_IDX);
	pio_set_pin_low(MDM_ONOFF_IDX);

	// Chek for power on
	while(true) {
		vTaskDelay(2500);

		if(pio_get_pin_value(MDM_POWMON_IDX) == 0) {
			break;
		} else {
			pio_set_pin_high(MDM_ENABLE_IDX);
			vTaskDelay(100);
			pio_set_pin_low(MDM_ENABLE_IDX);

		}
	}

	while(true) {
		pio_set_pin_high(MDM_ONOFF_IDX);
		vTaskDelay(2500);
		pio_set_pin_low(MDM_ONOFF_IDX);

		if(pio_get_pin_value(MDM_POWMON_IDX) == 1)
			break;

	}

}


uint8_t modem_init(void)
{
	// Hardware must be initialized before usart
	//init_hw();
	init_usart(BOARD_USART);

	const at_command_t* at_cmd = &(at_commands[0]);
	//SEND_AT((uint8_t*)at_cmd->cmd);

	while(at_cmd->cmd != NULL)
	{
		SEND_AT((uint8_t*)at_cmd->cmd);
		at_cmd++;

//		char * ptr = NULL;
//
//		if(handle_result(at_cmd->result, &ptr,  at_cmd->timeout) == SYS_OK)
//			at_cmd++;
//		else
//			return SYS_ERR_AT_FAIL;

//		for(u8_t r_time=0; r_time<cmd->retries; r_time++) {
//			SEND_AT(cmd->cmd);
//			if(handle_result(cmd->result, cmd->timeout) == SYS_OK) {
//				break;
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



static sys_err handle_result(char * token, char ** ptr_out, uint8_t seconds)
{
	char * ptr = NULL;

	//char * tmp_str = token;
	uint32_t time = sys_now();

	//u8_t ch;
	uint8_t received_char;
	uint8_t input_index = 0;
	static int8_t input_string[MAX_INPUT_SIZE];

	portTickType max_block_time_ticks = 200UL / portTICK_RATE_MS;

	input_index = 0;
	memset(input_string, '\0', MAX_INPUT_SIZE);

	// convert to seconds
	uint32_t timeout = (seconds*1000);

	//TODO: revise
	while((sys_now() - time) < timeout)
	{

		if (freertos_usart_serial_read_packet(modem_usart, &received_char, sizeof(received_char), portMAX_DELAY) == sizeof(received_char)) {

			if((ptr = strstr(input_string, token))) {
				if(ptr_out != NULL) {
					*ptr_out = ptr;
					return SYS_OK;
				}
			} else if (input_index < MAX_INPUT_SIZE) {
				input_string[input_index] = received_char;
				input_index++;
			}

		}
	}

	return SYS_ERR_AT_FAIL;
}

