/*
 * modem.h
 *
 *  Created on: Jun 16, 2015
 *      Author: jcobb
 */

#ifndef MODEM_H_
#define MODEM_H_
#include "freertos_usart_serial.h"

/* Dimensions the buffer into which input characters are placed. */
#define MAX_INPUT_SIZE          50

/* The size of the buffer provided to the USART driver for storage of received
 * bytes. */
#define RX_BUFFER_SIZE_BYTES    (50)

/* Baud rate to use. */
#define DIALER_BAUD_RATE           115200

/* The USART instance used for input and output. */
static freertos_usart_if modem_usart;


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


typedef struct {
	uint8_t type;

} dialer_cmd_t;

#endif /* MODEM_H_ */
