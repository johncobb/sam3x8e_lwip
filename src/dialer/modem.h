/*
 * modem.h
 *
 *  Created on: Jun 16, 2015
 *      Author: jcobb
 */

#ifndef MODEM_H_
#define MODEM_H_
#include "FreeRTOS.h"
#include "semphr.h"
#include "freertos_usart_serial.h"

/* Dimensions the buffer into which input characters are placed. */
#define MAX_INPUT_SIZE          128

/* The size of the buffer provided to the USART driver for storage of received
 * bytes. */
#define RX_BUFFER_SIZE_BYTES    (128)

/* Baud rate to use. */
#define DIALER_BAUD_RATE           115200

/* The USART instance used for input and output. */
extern freertos_usart_if modem_usart;


typedef enum
{
	SYS_OK = 0,
	SYS_AT_OK,
	SYS_ERR_AT_TIMEOUT,
	SYS_ERR_AT_NOCARRIER,
	SYS_ERR_AT_FAIL,
	SYS_ERR_FAIL_OTHERS
}sys_result;

uint8_t modem_init(void);
uint8_t modem_connect(void);


typedef struct {
	uint8_t type;

} dialer_cmd_t;

xSemaphoreHandle config_signal;

uint8_t modem_config(void);
uint32_t read_modem(void);

#endif /* MODEM_H_ */
