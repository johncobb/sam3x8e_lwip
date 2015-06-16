/*
 * dialer.h
 *
 *  Created on: Jun 10, 2015
 *      Author: jcobb
 */

#ifndef DIALER_H_
#define DIALER_H_
#include "FreeRTOS.h"
#include <stdint.h>
#include "semphr.h"
#include "queue.h"
#include "usart.h"

/* Dimensions the buffer into which input characters are placed. */
//#define MAX_INPUT_SIZE          50

/* The size of the buffer provided to the USART driver for storage of received
 * bytes. */
//#define RX_BUFFER_SIZE_BYTES    (50)

/* Baud rate to use. */
//#define DIALER_BAUD_RATE           115200


void create_dialer_task(uint16_t stack_depth_words, unsigned portBASE_TYPE task_priority);

xSemaphoreHandle dial_signal;




extern QueueHandle_t xDialerQueue;

#define CMD_TYPE_DIAL		1
#define CMD_TYPE_CLOSE		2
#define CMD_TYPE_RESET		3




#endif /* DIALER_H_ */
