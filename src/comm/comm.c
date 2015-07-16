/*
 * comm.c
 *
 *  Created on: Jul 8, 2015
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
#include "modem.h"
#include "comm.h"


QueueHandle_t xCommQueue;


comm_state_t comm_state = COMM_INIT;

static volatile uint8_t comm_dispatch_sig = 0;




static void comm_handler_task(void *pvParameters);

extern void comm_signal_handler(uint8_t sig)
{
	comm_dispatch_sig = 1;
}


void create_comm_task(uint16_t stack_depth_words, unsigned portBASE_TYPE task_priority)
{
	vSemaphoreCreateBinary(comm_signal);

	xCommQueue = xQueueCreate(10, sizeof(comm_frame_t *));


	xTaskCreate(	comm_handler_task,			/* The task that implements the command console. */
					(const int8_t *const) "COMM_D",	/* Text name assigned to the task.  This is just to assist debugging.  The kernel does not use this name itself. */
					stack_depth_words,					/* The size of the stack allocated to the task. */
					NULL,			/* The parameter is used to pass the already configured USART port into the task. */
					task_priority,						/* The priority allocated to the task. */
					NULL);

}

void comm_set_state(comm_state_t state)
{
	comm_state = state;
}

uint8_t socket_index = 0;
static BaseType_t result;

uint8_t comm_buffer[COMM_BUFFER_LEN+1] = {0};


static bool modem_ready = false;

static void comm_handler_task(void *pvParameters)
{
//	printf("modem_init\r\n");
//	modem_init();
//	printf("comm_connect\r\n");
//	comm_connect();
//	printf("comm_http\r\n");
//	comm_http();





	modem_socket_t * socket = &(modem_sockets[socket_index]);

	while(true) {


		// sanity check here
		if(modem_ready)
			modem_handler_async(20);

		// init the modem
		if(comm_state == COMM_INIT) {
			sys_result result = comm_init();
			if(result == SYS_OK){
				printf("modem ready!\r\n");
				modem_ready = true;
			}
		}

		// configure modem
		if(comm_state == COMM_CONFIG) {
			comm_config();
		}



		// register with network
		if(comm_state == COMM_REGISTER) {
			comm_register();
		}

		// take a breather
		if(comm_state == COMM_IDLE) {
			comm_idle();
		}

		// TODO: HACK TO TEST ABOVE CODE
		vTaskDelay(10);
		continue;

		if(comm_state == COMM_DISPATCH) {
			comm_dispatch(socket);
		}


		comm_frame_t *frame;

		result = xQueueReceive(xCommQueue, frame, QUEUE_TICKS);

		if(result == pdTRUE) {

			if(frame->type == FRAME_TCP)
			{
				// TODO: Enqueue to TCP thread
			}

			if(frame->type == FRAME_HTTP)
			{
				printf("frame received: %s\r\n", frame->buffer);
				// TODO: Enqueue to HTTP thread
			}

			if(frame->type == FRAME_UDP)
			{
				// TODO: Enqueue to UDP thread
			}
		}








//		if(comm_dispatch_sig > 0) {
//			comm_dispatch_sig = 0;
//			printf("dispatch signal received\r\n");
//		}

		vTaskDelay(100);
		printf("comm_handler_task loop\r\n");
	}

}
