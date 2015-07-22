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
#include "sys_arch.h"
#include "modem_defs.h"
#include "modem.h"
#include "dialer.h"
#include "ppp.h"
#include "comm.h"

static volatile bool cmd_dial = false;

QueueHandle_t xDialerQueue = NULL;

xSemaphoreHandle dial_signal = 0;

//http://read.pudn.com/downloads147/sourcecode/os/635985/LWIP%2BGTM900%2BLPC2104%E6%BA%90%E4%BB%A3%E7%A0%81/LPC2104_LWIP1.1_Source%20Code/code/main.c__.htm



static void ppp_dialer_task(void *pvParameters);
static void ppp_link_status_callback(void *ctx, int errCode, void *arg) ;

static void ppp_link_status_callback(void *ctx, int errCode, void *arg) {


	LWIP_UNUSED_ARG(ctx);

	switch(errCode) {
		case PPPERR_NONE: {             /* No error. */
			struct ppp_addrs *ppp_addrs = arg;

			printf("pppLinkStatusCallback: PPPERR_NONE\n");
			printf(" our_ipaddr=%s\n", ip_ntoa(&ppp_addrs->our_ipaddr));
			printf(" his_ipaddr=%s\n", ip_ntoa(&ppp_addrs->his_ipaddr));
			printf(" netmask   =%s\n", ip_ntoa(&ppp_addrs->netmask));
			printf(" dns1      =%s\n", ip_ntoa(&ppp_addrs->dns1));
			printf(" dns2      =%s\n", ip_ntoa(&ppp_addrs->dns2));
			break;
		}
		case PPPERR_PARAM: {           /* Invalid parameter. */
			printf("pppLinkStatusCallback: PPPERR_PARAM\n");
			break;
		}
		case PPPERR_OPEN: {            /* Unable to open PPP session. */
			printf("pppLinkStatusCallback: PPPERR_OPEN\n");
			break;
		}
		case PPPERR_DEVICE: {          /* Invalid I/O device for PPP. */
			printf("pppLinkStatusCallback: PPPERR_DEVICE\n");
			break;
		}
		case PPPERR_ALLOC: {           /* Unable to allocate resources. */
			printf("pppLinkStatusCallback: PPPERR_ALLOC\n");
			break;
		}
		case PPPERR_USER: {            /* User interrupt. */
			printf("pppLinkStatusCallback: PPPERR_USER\n");
			break;
		}
		case PPPERR_CONNECT: {         /* Connection lost. */
			printf("pppLinkStatusCallback: PPPERR_CONNECT\n");
			break;
		}
		case PPPERR_AUTHFAIL: {        /* Failed authentication challenge. */
			printf("pppLinkStatusCallback: PPPERR_AUTHFAIL\n");
			break;
		}
		case PPPERR_PROTOCOL: {        /* Failed to meet protocol. */
			printf("pppLinkStatusCallback: PPPERR_PROTOCOL\n");
			break;
		}
		default: {
			printf("pppLinkStatusCallback: unknown errCode %d\n", errCode);
			break;
		}
	}

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

uint8_t ppp_buffer[32+1];

static void ppp_dialer_task(void *pvParameters)
{
	int connected = 0;
	uint8_t received_char, input_index = 0, *output_string;
	static int8_t input_string[MAX_INPUT_SIZE],
			last_input_string[MAX_INPUT_SIZE];
	portBASE_TYPE returned_value;
	portTickType max_block_time_ticks = 200UL / portTICK_RATE_MS;

	printf("pppInit\r\n");
	pppInit();
	printf("pppSetAuth\r\n");
	//pppSetAuth(PPPAUTHTYPE_NONE, "", "");
	pppSetAuth(PPPAUTHTYPE_ANY, "", "");


	printf("modem_init\r\n");
	modem_init();
	printf("comm_connect\r\n");


	//comm_connect();

	comm_http();





	printf("pppOpen\r\n");

	int pppFd = pppOverSerialOpen(modem_usart, ppp_link_status_callback, &connected);

	printf("pppFd: %d\r\n", pppFd);

//	int loop_count = 0;
//
//	if(pppFd >=0) {
//
//		while(true) {
//			loop_count++;
//			if(loop_count == 60){
//				printf("timeout\r\n");
//				break;
//			}
//			if(connected) {
//				printf("ppp established\r\n");
//				break;
//			}
//			vTaskDelay(1000);
//
//		}
//
//		while(true) {
//			printf("nothing to do\r\n");
//			vTaskDelay(1000);
//		}
//	}


	/* end handling ppp communications */

	uint8_t len = 0;

	printf("entering loop\r\n");

	while(true) {

		memset(ppp_buffer, '0', 32+1);

		portTickType max_wait_millis = 20 / portTICK_RATE_MS;


		len = freertos_usart_serial_read_packet(modem_usart, ppp_buffer, 32, max_wait_millis);
		//len = read_modem();
		//printf("len: %d\r\n", len);
		if(len>0) {
			pppos_input(pppFd, ppp_buffer, len);

		}

		//vTaskDelay(50);
	}



//	lwip_init();
//
//	uint8_t MYDATALEN = 128;
//	uint8_t mydata[MYDATALEN];
//
//	struct tcp_pcb *pcb;
//	struct ip_addr ipaddr;
//	uint16_t port;
//	err_t err = ERR_CONN;
//
//
//	pcb = tcp_new();
//	IP4_ADDR(&ipaddr, 10,10, 10, 100);
//	port = 80;
//
//	err = tcp_connect(pcb, &ipaddr, port, tcp_connected);
//
//	if(err != ERR_OK) {
//		printf("tcp_connecte returned error: %d\r\n", err);
//	} else {
//		printf("connected\r\n");
//	}
//
//	//tcp_connect();
//	tcp_write(pcb, mydata, MYDATALEN, 0);
//	//tcp_send(pcb);




	/* end handling ppp communications */








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


