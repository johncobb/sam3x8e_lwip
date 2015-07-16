/*
 * comm_dispatch.c

 *
 *  Created on: Jul 13, 2015
 *      Author: jcobb
 */
#include <string.h>
#include "modem_defs.h"
#include "telit_modem_api.h"
#include "comm.h"
#include "comm_dispatch.h"
#include "http_handler.h"



//uint8_t socket_index = 0;

typedef enum
{
	COMM_SOCKET_OPEN = 0,
	COMM_SOCKET_SUSPEND,
	COMM_SOCKET_RESUME,
	COMM_SOCKET_WRITE,
	COMM_SOCKET_WAITREPLY,
	COMM_SOCKET_LISTEN,
	COMM_SOCKET_STATUS,
	COMM_SOCKET_CLOSE
}comm_dispatch_state;

void comm_settimeout(uint32_t millis);
bool comm_checkfortimeout(void);

void comm_dispatch_enterstate(comm_dispatch_state state, uint32_t timeout);
void comm_dispatch_exitstate();

comm_dispatch_state dispatch_state_index = COMM_SOCKET_OPEN;
uint8_t sub_state_index = 0;


xTimeOutType time_out_definition;
portTickType max_wait_millis;

void comm_settimeout(uint32_t millis)
{
	max_wait_millis = millis / portTICK_RATE_MS;

	/* Remember the time on entry. */
	vTaskSetTimeOutState(&time_out_definition);

}

bool comm_checkfortimeout(void)
{
	bool timeout = false;

	if (xTaskCheckForTimeOut(&time_out_definition, &max_wait_millis) == pdTRUE)
	{
		printf("comm_timeout.\r\n");
		timeout = true;
	}

	return timeout;
}

void comm_dispatch_enterstate(comm_dispatch_state state, uint32_t timeout)
{

	if(timeout > 0)
		comm_settimeout(timeout);

	dispatch_state_index = state;
	sub_state_index = 0;
}

void comm_dispatch_exitstate(void)
{
	dispatch_state_index = COMM_SOCKET_RESUME;
	sub_state_index = 0;
}

//sys_result  comm_dispatch(void)
sys_result  comm_dispatch(modem_socket_t * socket)
{

//	modem_socket_t * socket = &(modem_sockets[socket_index]);

//	char *buffer = NULL;


	while(true)
	{
		// STEP 1 try to resume an existing connection
		// continue loop
		if(dispatch_state_index == COMM_SOCKET_RESUME) {

			printf("modem_socketresume...\r\n");
			modem_socketresume(socket);

			sys_result sys_status = modem_handle_socketresume();

			if(sys_status == SYS_AT_OK) {
				comm_dispatch_enterstate(COMM_SOCKET_WRITE, 0);
			} else {
				comm_dispatch_enterstate(COMM_SOCKET_OPEN, 0);
			}
			continue;
		}

		// STEP 2 if resume fails then open a new connection
		// continue loop
		if(dispatch_state_index == COMM_SOCKET_OPEN) {

			printf("modem_socketopen...\r\n");
			modem_socketopen(socket);


			sys_result sys_status = modem_handle_socketopen();

			if(sys_status == SYS_AT_OK) {
				comm_dispatch_enterstate(COMM_SOCKET_WRITE, 0);
				//comm_dispatch_enterstate(COMM_SOCKET_SUSPEND);
				continue;
			} else {
				comm_dispatch_enterstate(COMM_SOCKET_SUSPEND, 0);
				printf("failed: sys_status=%d\r\n", sys_status);
				// TODO: POSSIBLY IMPLEMENT RETRY
			}
			continue;
		}

		// STEP 3a write the buffer to the socket and await the response
		// continue loop
		if(dispatch_state_index == COMM_SOCKET_WRITE) {


			printf("modem_socketwrite...\r\n");
			char * data = MODEM_DEFAULT_HTTPREQUEST;

			memset(socket->data_buffer, '\0', SOCKET_BUFFER_LEN+1);
			socket->bytes_received = 0;
			modem_socketwrite(socket, data);
			comm_dispatch_enterstate(COMM_SOCKET_WAITREPLY, 5000);
			continue;

		}

		if(dispatch_state_index == COMM_SOCKET_WAITREPLY) {


			if(comm_checkfortimeout())
				comm_dispatch_enterstate(COMM_SOCKET_SUSPEND, 0);
//			portTickType max_wait_millis = 20 / portTICK_RATE_MS;
//			xTimeOutType time_out_definition;
//			/* Remember the time on entry. */
//			vTaskSetTimeOutState(&time_out_definition);
//			if (xTaskCheckForTimeOut(&time_out_definition, &max_wait_millis) == pdTRUE) {
//
//			}
			sys_result sys_status = modem_handle_socketwrite(socket);

			if(socket->bytes_received > 0)
			{
				socket->handle_data(socket->data_buffer, socket->bytes_received);
			}

			continue;
		}

		// STEP 3b socket listen
		// continue loop
//		if(dispatch_state_index == COMM_SOCKET_LISTEN) {
//
//			// TODO: REWORK
//			char * data = "GET / HTTP/1.1\r\nHost: www.google.com\r\nConnection: keep-alive\r\n\r\n";
//			char * response = MODEM_TOKEN_HTTPOK;
//
//			modem_socketlisten(socket);
//
//			sys_result sys_status = modem_handle_socketlisten(response);
//
//			if(sys_status == SYS_AT_OK) {
//				comm_dispatch_enterstate(COMM_SOCKET_SUSPEND);
//				continue;
//			} else {
//				// TODO: POSSIBLY IMPLEMENT RETRY
//			}
//		}

		// STEP 4 suspend the connection and set comm state back to COMM_IDLE
		// break loop
		if(dispatch_state_index == COMM_SOCKET_SUSPEND) {

			modem_socketsuspend(socket);

			sys_result sys_status = modem_handle_socketsuspend();

			if(sys_status == SYS_AT_OK) {
				comm_dispatch_exitstate();
				comm_set_state(COMM_IDLE);
				break;
			} else if (sys_status == SYS_ERR_AT_NOCARRIER) {
				comm_dispatch_exitstate();
				comm_set_state(COMM_IDLE);
				break;
			}else if (sys_status == SYS_ERR_AT_TIMEOUT) {
				comm_dispatch_exitstate();
				comm_set_state(COMM_IDLE);
				break;
			}
		}

		// throttle
		vTaskDelay(50);
	}

	return SYS_OK;
}





