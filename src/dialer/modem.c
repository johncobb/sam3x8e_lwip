/*
 * modem.c
 *
 *  Created on: Jun 16, 2015
 *      Author: jcobb
 */

#include <stdint.h>
#include <string.h>
#include "FreeRTOS.h"
#include "semphr.h"
#include "modem.h"
#include "modem_defs.h"
#include "freertos_usart_serial.h"
#include "board.h"
#include "sys_arch.h"
#include "telit_modem_api.h"
#include "http_handler.h"


#define RX_BUFFER_LEN		128

freertos_usart_if modem_usart;

uint8_t modem_rx_buffer_index = 0;
static uint8_t modem_rx_buffer[MODEM_RX_BUFFER_SIZE+1] = {0};

modem_status_t modem_status;

xSemaphoreHandle config_signal = 0;

void basic_handler(void);

void pause(void);



//"AT#SCFG=1,1,512,90,600,2\r"
modem_socket_t modem_sockets[] =
{
		// cnx_id, ctx_id, pkt_size, glb_timeout, cnx_timeout (tenths of second), tx_timeout (tenths of second)
		// socket_status, endpoint
		// protocol, address, port
		// function_handler, data_buffer, bytes_received
		//{1, 1, 512, 90, 600, 2, 0, {SOCKET_TCP, SOCKET_PROT_HTTP, 80}, http_handle_data, {0}, 0},
		{1, 1, 512, 90, 600, 2, 0, {0}, {SOCKET_TCP, SOCKET_PROT_HTTP, 1888}, http_handle_data, {0}, 0},
		{2, 1, 512, 90, 600, 2, 0, {0}, {SOCKET_TCP, SOCKET_PROT_HTTP, 1888}, http_handle_data, {0}, 0},
//		{2, 1, 512, 90, 600, 2, 0, {0}, {SOCKET_TCP, SOCKET_PROT_TCP, 80}, NULL, {0}, 0},
//		{3, 1, 512, 90, 600, 2},
//		{4, 1, 512, 90, 600, 2},
//		{5, 1, 512, 90, 600, 2},
//		{6, 1, 512, 90, 600, 2},
		{0, 0, 0, 0, 0}

};


uint32_t timeout = 0;
//char received_char;
//int8_t input_index = 0;
//static char input_string[MAX_INPUT_SIZE];
char * ptr = NULL;

uint32_t bytes_received;
uint8_t rx_buffer[RX_BUFFER_LEN+1];



void reset_rx_buffer(void)
{
	bytes_received = 0;
	memset(modem_rx_buffer, '\0', sizeof(modem_rx_buffer));
}

void basic_handler(void)
{
	printf("basic_handler\r\n");
}

void pause(void)
{
	printf("pausing 10 sec.\r\n");
	vTaskDelay(10000);
	printf("resuming...\r\n");
}




/* The buffer provided to the USART driver to store incoming character in. */
static uint8_t receive_buffer[RX_BUFFER_SIZE_BYTES] = {0};

static void init_hw(void);
static void init_usart(Usart *usart_base);



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


sys_result modem_init(void)
{
	// hardware must be initialized before usart
	init_hw();

	// configure usart pins (must be done after init_hw call)
	board_init_modem_usart();

	// init usart
	init_usart(MODEM_USART);

	return SYS_OK;
}

//sys_result modem_config(uint8_t config_index)
//{
//	at_modem_cmd_t * at_cmd = &(at_cfg_commands[config_index]);
//
//	printf("wait .5s\r\n");
//	vTaskDelay(500);
//
//	while(at_cmd->fnc_handler != NULL)
//	{
//
//		// sanity check here
//		// if we've reached the end of the array all config functions have been called
//		// so return SYS_CONFIG_OK
//		if(at_cmd->fnc_handler == NULL) return SYS_CONFIG_OK;
//
//		// dispatch the function
//		at_cmd->fnc_handler();
//
//		// advance to next command
//		at_cmd++;
//
//		char * ptr = NULL;
//
//		sys_result sys_status;
//
//		if(at_cmd->fnc_callback != NULL) {
//			at_cmd->fnc_callback();
//		} else {
//			sys_status = handle_result(MODEM_TOKEN_OK, &ptr,  1000);
//		}
//
//		vTaskDelay(50);
//
//
//		if(sys_status == SYS_AT_OK) {
//			//at_cmd++;
//			printf("SYS_OK\r\n");
//			printf("buffer:\r\n%s\r\n", ptr);
//			continue;
//		} else if (sys_status == SYS_ERR_AT_FAIL) {
//			printf("SYS_ERR_AT_FAIL\r\n");
//		} else if (sys_status == SYS_ERR_AT_NOCARRIER) {
//			printf("SYS_ERR_AT_NO_CARRIER\r\n");
//		} else if(sys_status == SYS_ERR_AT_TIMEOUT) {
//			printf("SYS_ERR_AT_TIMEOUT\r\n");
//			printf("buffer:\r\n%s\r\n", ptr);
//			//return SYS_ERR_AT_FAIL;
//		}
//
//	}
//
//	return SYS_OK;
//}


sys_result modem_config_handler(void)
{
	char * ptr = NULL;

	sys_result sys_status;

	sys_status = handle_result(MODEM_TOKEN_OK, &ptr);

	if(sys_status == SYS_AT_OK) {
		//at_cmd++;
		printf("SYS_OK\r\n");
		printf("buffer:\r\n%s\r\n", ptr);
	} else if (sys_status == SYS_ERR_AT_FAIL) {
		printf("SYS_ERR_AT_FAIL\r\n");
	} else if (sys_status == SYS_ERR_AT_NOCARRIER) {
		printf("SYS_ERR_AT_NO_CARRIER\r\n");
	} else if(sys_status == SYS_ERR_AT_TIMEOUT) {
		printf("SYS_ERR_AT_TIMEOUT\r\n");
		printf("buffer:\r\n%s\r\n", ptr);
		//return SYS_ERR_AT_FAIL;
	}

	return sys_status;
}


sys_result configure_sockets(void)
{
	printf("configure_sockets...\r\n");
	modem_socket_t * socket = &(modem_sockets[0]);

	while(socket->cnx_id > 0)
	{
		char * ptr = NULL;

		sys_result sys_status;

		// cnx_id, ctx_id, pkt_size, glb_timeout, cnx_timeout (tenths of second), tx_timeout (tenths of second)
		modem_socketconfigex(socket);

		sys_status = handle_result(MODEM_TOKEN_OK, &ptr);

		socket++;

		if(sys_status == SYS_AT_OK) {
			printf("SYS_OK\r\n");
			printf("buffer:\r\n%s\r\n", ptr);
			continue;
		} else if (sys_status == SYS_ERR_AT_FAIL) {
			printf("SYS_ERR_AT_FAIL\r\n");
		} else if (sys_status == SYS_ERR_AT_NOCARRIER) {
			printf("SYS_ERR_AT_NO_CARRIER\r\n");
		} else if(sys_status == SYS_ERR_AT_TIMEOUT) {
			printf("SYS_ERR_AT_TIMEOUT\r\n");
			printf("buffer:\r\n%s\r\n", ptr);
			//return SYS_ERR_AT_FAIL;
		}

	}

	return SYS_OK;
}





static void init_usart(Usart *usart_base)
{

	freertos_peripheral_options_t driver_options = {
		receive_buffer,									/* The buffer used internally by the USART driver to store incoming characters. */
		RX_BUFFER_SIZE_BYTES,							/* The size of the buffer provided to the USART driver to store incoming characters. */
		configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY,	/* The priority used by the USART interrupts. */
		USART_RS232,									/* Configure the USART for RS232 operation. */
		//UART_RS232,
		//(WAIT_TX_COMPLETE | USE_TX_ACCESS_MUTEX)		/* Use access mutex on Tx (as more than one task transmits) but not Rx. Wait for a Tx to complete before returning from send functions. */
		(USE_RX_ACCESS_MUTEX | USE_TX_ACCESS_MUTEX)
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


void SEND_AT(uint8_t *cmd)
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

void SEND_RAW(uint8_t *cmd)
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



uint32_t modem_handler_async(uint32_t millis)
{
	portTickType max_wait_millis = millis / portTICK_RATE_MS;

	uint32_t len = freertos_usart_serial_read_packet(modem_usart, modem_rx_buffer, MODEM_RX_BUFFER_SIZE, max_wait_millis);

	//printf("modem_handler_async: %lu\r\n", len);
	return len;
}

sys_result handle_result(char * token, char ** ptr_out)
{


	// SPECIAL CASE FOR HANDLING ASYNCHRONOUS DATA
//	if(token == NULL) {
//		*ptr_out = modem_rx_buffer;
//		return SYS_OK;
//	}

	//if((ptr = strstr(input_string, token))) {
	if((ptr = strstr(modem_rx_buffer, token))) {
		if(ptr_out != NULL) {
			*ptr_out = ptr;
		}
		//printf("SYS_AT_OK\r\n");
		return SYS_AT_OK;
	} else if ((ptr = strstr(modem_rx_buffer, MODEM_TOKEN_ERROR))) {
		if(ptr_out != NULL) {
			*ptr_out = ptr;
		}
		printf("SYS_ERR_AT_FAIL\r\n");
		return SYS_ERR_AT_FAIL;
	} else if((ptr = strstr(modem_rx_buffer, MODEM_TOKEN_NOCARRIER))) {
		if(ptr_out != NULL) {
			*ptr_out = ptr;
		}
		printf("SYS_ERR_AT_NOCARRIER\r\n");
		return SYS_ERR_AT_NOCARRIER;
	}


	// set ptr_out to the rx_buffer for troubleshooting
	if(ptr_out != NULL) {
		*ptr_out = modem_rx_buffer;
	}

}

sys_result handle_result2(char * token, char ** ptr_out, uint32_t millis)
{

	portTickType max_wait_millis = millis / portTICK_RATE_MS;

	memset(rx_buffer, '\0', RX_BUFFER_LEN+1);

	bytes_received = freertos_usart_serial_read_packet(modem_usart, rx_buffer, RX_BUFFER_LEN, max_wait_millis);

	if (bytes_received > 0) {

		// SPECIAL CASE FOR HANDLING ASYNCHRONOUS DATA
		if(token == NULL) {
			*ptr_out = rx_buffer;
			return SYS_OK;
		}

		//if((ptr = strstr(input_string, token))) {
		if((ptr = strstr(rx_buffer, token))) {
			if(ptr_out != NULL) {
				*ptr_out = ptr;
			}
			//printf("SYS_AT_OK\r\n");
			return SYS_AT_OK;
		} else if ((ptr = strstr(rx_buffer, MODEM_TOKEN_ERROR))) {
			if(ptr_out != NULL) {
				*ptr_out = ptr;
			}
			printf("SYS_ERR_AT_FAIL\r\n");
			return SYS_ERR_AT_FAIL;
		} else if((ptr = strstr(rx_buffer, MODEM_TOKEN_NOCARRIER))) {
			if(ptr_out != NULL) {
				*ptr_out = ptr;
			}
			printf("SYS_ERR_AT_NOCARRIER\r\n");
			return SYS_ERR_AT_NOCARRIER;
		}
	}

	// set ptr_out to the rx_buffer for troubleshooting
	if(ptr_out != NULL) {
		*ptr_out = rx_buffer;
	}

	printf("SYS_ERR_AT_TIMEOUT\r\n");
	return SYS_ERR_AT_TIMEOUT;
}


uint32_t handle_stream(uint8_t *data, uint32_t len, uint32_t millis)
{
	portTickType max_wait_millis = millis / portTICK_RATE_MS;

	bytes_received = freertos_usart_serial_read_packet(modem_usart, data, len, max_wait_millis);

	return bytes_received;
}

uint32_t read_modem(void)
{
	portTickType max_wait_millis = 20 / portTICK_RATE_MS;

	memset(rx_buffer, '\0', RX_BUFFER_LEN+1);
	return freertos_usart_serial_read_packet(modem_usart, rx_buffer, RX_BUFFER_LEN, max_wait_millis);
}

uint32_t modem_copy_buffer(uint8_t *data)
{
	memcpy(data, modem_rx_buffer, bytes_received);
	return bytes_received;
}


//static sys_result handle_result(char * token, char ** ptr_out, uint32_t millis)
//{
//
//
//	//portTickType max_block_time_ticks = 200UL / portTICK_RATE_MS;
//	portTickType max_wait_millis = millis / portTICK_RATE_MS;
//	input_index = 0;
//	memset(input_string, '\0', MAX_INPUT_SIZE);
//	memset(rx_buffer, '\0', RX_BUFFER_LEN+1);
//
//	timeout = ((uint32_t)sys_now()) + (1000);
//
//	while(true)
//	{
//		// check for timeout
//		if(sys_now() >= timeout) {
//			printf("at timeout\r\n");
//			break;
//		}
//
//		bytes_received = freertos_usart_serial_read_packet(modem_usart, rx_buffer, RX_BUFFER_LEN, max_wait_millis);
//
//		if (bytes_received > 0) {
//
//			//printf("received %c", (char) received_char);
//
//
//			//if((ptr = strstr(input_string, token))) {
//			if((ptr = strstr(rx_buffer, token))) {
//				if(ptr_out != NULL) {
//					*ptr_out = ptr;
//					//printf("sys_ok\r\n");
//				}
//				return SYS_AT_OK;
//			} else if ((ptr = strstr(input_string, MODEM_TOKEN_ERROR))) {
//				*ptr_out = ptr;
//				return SYS_ERR_AT_FAIL;
//			} else if((ptr = strstr(input_string, MODEM_TOKEN_NOCARRIER))) {
//				*ptr_out = ptr;
//				return SYS_ERR_AT_NOCARRIER;
//			} else if (input_index < MAX_INPUT_SIZE) {
//				if(received_char == '\0')
//					continue;
//				input_string[input_index] = received_char;
//				//printf("buffer: %s\r\n", input_string);
//				input_index++;
//			}
//
//		}
//	}
//	// set ptr_out to the char array
//	*ptr_out = input_string;
//	//printf("sys_err_at_fail\r\n");
//	return SYS_ERR_AT_TIMEOUT;
//}



//void init_modem_buffer(void);
//void init_modem_lines(void);
////sys_result process_modem(char * token, char ** ptr_out, uint8_t seconds);
//sys_result read_modem_usart(uint8_t seconds);
//
//
//uint8_t modem_lines[MAX_INPUT_SIZE+1];
//uint8_t modem_line_index = 0;
//
//uint8_t modem_buffer[MAX_INPUT_SIZE+1];
//uint8_t modem_buffer_index = 0;
//
//sys_result handle_result(char * token, char ** ptr_out, uint8_t seconds)
//{
//	init_modem_lines();
//	if(read_modem_usart(seconds) == SYS_AT_OK) {
//
//		char * ptr = NULL;
//
//		if((ptr = strstr(modem_lines, token))) {
//			if(ptr_out != NULL) {
//				*ptr_out = ptr;
//				//printf("sys_ok\r\n");
//				return SYS_AT_OK;
//			}
//		} else if ((ptr = strstr(modem_lines, MODEM_TOKEN_ERROR))) {
//			*ptr_out = ptr;
//			return SYS_ERR_AT_FAIL;
//		} else if((ptr = strstr(modem_lines, MODEM_TOKEN_NOCARRIER))) {
//			*ptr_out = ptr;
//			return SYS_ERR_AT_NOCARRIER;
//		}
//	}
//
//	return SYS_ERR_AT_TIMEOUT;
//}
//
//sys_result read_modem_usart(uint8_t seconds)
//{
//	uint8_t c;
//	uint8_t buffer_index = 0;
//
//	// convert to seconds
//	uint32_t timeout = ((uint32_t)sys_now()) + (seconds*1000);
//	//portTickType max_block_time_ticks = 200UL / portTICK_RATE_MS;
//
//	while(sys_now() <= timeout)
//	{
//		if (freertos_usart_serial_read_packet(modem_usart, &c, sizeof(c), portMAX_DELAY) == sizeof(c)) {
//
//			if(c == '\0')
//				continue;
//
//			modem_buffer[buffer_index] = c;
//			buffer_index++;
//
//			if(buffer_index >= MAX_INPUT_SIZE)
//				continue;
//
//			if (c == '\r' || c == '\n') {
//				strcpy(modem_buffer, modem_lines);
//				init_modem_buffer();
//				return SYS_AT_OK;
//			}
//		}
//	}
//
//	return SYS_ERR_AT_TIMEOUT;
//}
//
//void init_modem_buffer(void)
//{
//	modem_line_index = 0;
//	memset(modem_buffer, '\0', sizeof(modem_buffer));
//}
//
//void init_modem_lines(void)
//{
//	memset(modem_lines, '\0', sizeof(modem_lines));
//}

