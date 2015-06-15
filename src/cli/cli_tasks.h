/*
 * cli_tasks.h
 *
 *  Created on: Jun 1, 2015
 *      Author: jcobb
 */

#ifndef CLI_TASKS_H_
#define CLI_TASKS_H_
#include <stdint.h>

#include "uart.h"


void create_uart_cli_task(Uart *uart_base, uint16_t stack_depth_words,
		unsigned portBASE_TYPE task_priority);
void uart_cli_output(const uint8_t *message_string);

#endif /* CLI_TASKS_H_ */
