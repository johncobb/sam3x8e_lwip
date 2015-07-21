/*
 * tcpip.c
 *
 *  Created on: Jul 21, 2015
 *      Author: jcobb
 */
#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "comm.h"
#include "tcpip.h"

xSemaphoreHandle connect_signal = 0;

void new_connection(comm_request_t *request, uint8_t *endpoint);
//
//xSemaphoreGive(connect_signal);
//
//vSemaphoreCreateBinary(connect_signal);
//
//if(xSemaphoreTake(connect_signal, portMAX_DELAY)) {
//
//}


void new_connection(comm_request_t *request, uint8_t *endpoint)
{
	printf("new_connection()\r\n");
	// sanity check, fill buffer with null terminated strings
	memset(request->endpoint, '\0', FRAME_BUFFER_LEN+1);

	request->socket_address = 0;
	request->type = REQUEST_CONNECT;
	request->timeout = DEFAULT_TCIP_CONNECTTIMEOUT;

	// copy in the endpoint address
	memcpy(request->endpoint, endpoint, SOCKET_ENDPOINT_LEN);

}


//tcp_result cph_tcp_connect(char *endpoint, uint32_t len)
//{
//
//	// TODO: SEMAPHORE TAKE GIVE TASK NOTIFY
//	BaseType_t result;
//
//	comm_request_t request;
//
//	memset(request.endpoint, '\0', FRAME_BUFFER_LEN+1);
//	request.socket_address = 0;
//	request.type = REQUEST_CONNECT;
//	request.timeout = DEFAULT_TCIP_CONNECTTIMEOUT;
//
//	// copy in the destination address
//	//memcpy(request.endpoint, MODEM_DEFAULT_HTTPSERVER, SOCKET_ENDPOINT_LEN);
//	memcpy(request.endpoint, MODEM_DEFAULT_HTTPSERVER, SOCKET_ENDPOINT_LEN);
//
//	//printf("enqueueing endpoint: %s\r\n", request.endpoint);
//
//	result = xQueueSendToBack( xCommQueueRequest, &request, (TickType_t)0);
//
//	if(result == pdTRUE) {
//		printf("request enqueued.\r\n");
//	}
//
//	if(xSemaphoreTake(connect_signal, portMAX_DELAY)) {
//		printf("tcp_connect: connected.\r\n");
//	}
//
//
//	return SYS_TCP_OK;
//}

tcp_result cph_tcp_connect(uint8_t *endpoint, uint32_t len)
{

	// TODO: SEMAPHORE TAKE GIVE TASK NOTIFY
	BaseType_t result;

	comm_request_t request;

	new_connection(&request, MODEM_DEFAULT_HTTPSERVER);

	result = xQueueSendToBack( xCommQueueRequest, &request, (TickType_t)0);

	if(result == pdTRUE) {
		printf("request enqueued.\r\n");
	}

	if(xSemaphoreTake(connect_signal, portMAX_DELAY)) {
		printf("tcp_connect: connected.\r\n");
	}


	return SYS_TCP_OK;
}



