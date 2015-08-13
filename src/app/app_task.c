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

xSemaphoreHandle app_start_signal = 0;


static volatile bool start_task = false;
static volatile bool wait_ack = false;

static void app_handler_task(void *pvParameters);

void create_app_task(uint16_t stack_depth_words, unsigned portBASE_TYPE task_priority)
{

	vSemaphoreCreateBinary(app_start_signal);

	xTaskCreate(	app_handler_task,			/* The task that implements the command console. */
					(const int8_t *const) "APP",	/* Text name assigned to the task.  This is just to assist debugging.  The kernel does not use this name itself. */
					stack_depth_words,					/* The size of the stack allocated to the task. */
					NULL,			/* The parameter is used to pass the already configured USART port into the task. */
					task_priority,						/* The priority allocated to the task. */
					NULL);

}

static void app_data_handler(uint8_t *data, uint32_t len);
static sys_result parse_result(char * buffer, char * token, char ** ptr_out);

void app_start(void)
{
	printf("app_start\r\n");
	if(xSemaphoreTake(app_start_signal, portMAX_DELAY)) {
		start_task = true;
	}
}


void app_data_handler(uint8_t *data, uint32_t len)
{
	printf("app_data_handler: %s\r\n", data);

	if(wait_ack) {
		char * ptr = NULL;
		sys_result result = parse_result(data, "ACK", &ptr);

		if(result == SYS_OK) {
			printf("ack found!\r\n");
		}
	}
}

static sys_result parse_result(char * buffer, char * token, char ** ptr_out)
{
	sys_result result;

	char * ptr = NULL;

	if((ptr = strstr(buffer, token))) {
		if(ptr_out != NULL) {
			*ptr_out = ptr;
		}
		//printf("SYS_AT_OK\r\n");
		result = SYS_OK;
	} else {
		result = SYS_NOTFOUND;
	}

	return result;
}


static void app_handler_task(void *pvParameters)
{

	uint8_t * ip_endpoint = "bs.cphandheld.com";
	uint8_t * packet = "20000000,01,01,00000007802F6399,E16B,00000007802DE16B,1973,16,0001,0000,00C3,0000,0001,EE000000,470000EE\r";

	socket_connection_t sck_connection;

	xSemaphoreGive(app_start_signal);

	while(true) {

		if(start_task) {
			start_task = false;

			printf("start task.\r\n");

			// create a new socket connection
			socket_newconnection(&sck_connection, ip_endpoint, DEFAULT_TCIP_CONNECTTIMEOUT);

			// establish a connection
			tcp_result result = cph_tcp_connect(&sck_connection);

			if(result == SYS_TCP_OK) {
				printf("successfully connected.\r\n");

				// send a message once per second
				while(true) {
					result = cph_tcp_send(&sck_connection, packet, app_data_handler);
					vTaskDelay(1000);
				}
			}
		}
	}


	while(true) {

		vTaskDelay(500);
	}
}
