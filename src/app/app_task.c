/*
 * app_task.c
 *
 *  Created on: Jul 22, 2015
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
//#include "modem_defs.h"
//#include "modem.h"
//#include "comm.h"
#include "tcpip.h"
#include "app_task.h"

static void app_handler_task(void *pvParameters);

void create_app_task(uint16_t stack_depth_words, unsigned portBASE_TYPE task_priority)
{


	xTaskCreate(	app_handler_task,			/* The task that implements the command console. */
					(const int8_t *const) "APP",	/* Text name assigned to the task.  This is just to assist debugging.  The kernel does not use this name itself. */
					stack_depth_words,					/* The size of the stack allocated to the task. */
					NULL,			/* The parameter is used to pass the already configured USART port into the task. */
					task_priority,						/* The priority allocated to the task. */
					NULL);

}


static void app_handler_task(void *pvParameters)
{

	uint8_t state = 0;

	while(true) {

//		if(state == 0) {
//			cph_tcp_connect("bs.cphandheld.com" );
//			state++;
//		} else if (state == 1) {
//
//		}
		printf("app_task\r\n");
		vTaskDelay(500);
	}
}
