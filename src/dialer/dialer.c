/*
 * dialer.c
 *
 *  Created on: Jun 10, 2015
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
#include "dialer.h"
#include "ppp.h"

static volatile bool cmd_dial = false;

QueueHandle_t xDialerQueue = NULL;

xSemaphoreHandle dial_signal = 0;

//http://read.pudn.com/downloads147/sourcecode/os/635985/LWIP%2BGTM900%2BLPC2104%E6%BA%90%E4%BB%A3%E7%A0%81/LPC2104_LWIP1.1_Source%20Code/code/main.c__.htm



static void ppp_dialer_task(void *pvParameters);


static void init_ppp(void);

volatile u32_t _sys_jiffies_ = 0;

void sys_init(void)
{
	_sys_jiffies_ = sys_now();
}

u32_t sys_now(void)
{
	return (u32_t) xTaskGetTickCount();
}

u32_t sio_write(sio_fd_t fd, u8_t *data, u32_t len)
{
	return 0;
}


u32_t sys_jiffies(void)
{
	return _sys_jiffies_;
}

void modem_error_cb(void *ctx, int errCode, void *arg) {

}




void create_dialer_task(uint16_t stack_depth_words, unsigned portBASE_TYPE task_priority)
{
	vSemaphoreCreateBinary(dial_signal);

	sys_init();

	/* Create the USART CLI task. */
//	xTaskCreate(	ppp_dialer_task,			/* The task that implements the command console. */
//					(const int8_t *const) "PPP_D",	/* Text name assigned to the task.  This is just to assist debugging.  The kernel does not use this name itself. */
//					stack_depth_words,					/* The size of the stack allocated to the task. */
//					(void *) freertos_usart,			/* The parameter is used to pass the already configured USART port into the task. */
//					task_priority,						/* The priority allocated to the task. */
//					NULL);

	xTaskCreate(	ppp_dialer_task,			/* The task that implements the command console. */
					(const int8_t *const) "PPP_D",	/* Text name assigned to the task.  This is just to assist debugging.  The kernel does not use this name itself. */
					stack_depth_words,					/* The size of the stack allocated to the task. */
					NULL,			/* The parameter is used to pass the already configured USART port into the task. */
					task_priority,						/* The priority allocated to the task. */
					NULL);

}

static void ppp_dialer_task(void *pvParameters)
{
	uint8_t received_char, input_index = 0, *output_string;
	static int8_t input_string[MAX_INPUT_SIZE],
			last_input_string[MAX_INPUT_SIZE];
	portBASE_TYPE returned_value;
	portTickType max_block_time_ticks = 200UL / portTICK_RATE_MS;


	pppInit();
	// TODO: init string and dial the modem

	modem_init();

	pppSetAuth(PPPAUTHTYPE_NONE, "", "");

	void * link_status_ctx;

	int pppFd = pppOpen((sio_fd_t) modem_usart, modem_error_cb, link_status_ctx);



	/* Obtain the address of the output buffer.  Note there is no mutual
	exclusion on this buffer as it is assumed only one command console
	interface will be used at any one time. */

//	static char * tmp_buffer[400];
//
//	output_string = (uint8_t *) tmp_buffer;

	/* Send the welcome message.  The message is copied into RAM first as the
	DMA cannot send from Flash addresses. */
//	strcpy((char *) output_string, (char *) dialer_message);
//	freertos_usart_write_packet(modem_usart, output_string,
//			strlen((char *) dialer_message),
//			max_block_time_ticks);


	BaseType_t result;


	for(;;) {

		//result = xQueueReceive(xDialerQueue, outBuffer, QUEUE_TICKS);

		//if(result == pdTRUE) {
		if(xSemaphoreTake(dial_signal, portMAX_DELAY)) {

			//strcpy((char *) output_string, (char *) dialer_cmd_at_dial);
			//freertos_usart_write_packet(modem_usart, output_string, strlen((char *) dialer_cmd_at_dial), max_block_time_ticks);
		}

//		if(cmd_dial) {
//			// toggle our flag
//			cmd_dial = false;
//			strcpy((char *) output_string, (char *) dialer_cmd_dial);
//			freertos_usart_write_packet(cli_usart, output_string, strlen((char *) dialer_cmd_dial), max_block_time_ticks);
//		}



		//vTaskDelay(200);
	}
}

extern void dial(void)
{
	cmd_dial = true;
}


