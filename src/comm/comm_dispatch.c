/*
 * comm_dispatch.c

 *
 *  Created on: Jul 13, 2015
 *      Author: jcobb
 */
#include <string.h>
#include "modem_defs.h"
#include "modem.h"
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
}comm_dispatch_state_t;

typedef enum
{
	COMM_DISPATCH_INVOKE = 0,
	COMM_DISPTCH_WAITREPLY = 1
}comm_dispatch_sub_state_t;


static comm_dispatch_state_t _state = COMM_SOCKET_RESUME;
comm_dispatch_sub_state_t _substate = COMM_DISPATCH_INVOKE;

static void set_timeout(uint32_t millis);
static bool timeout(void);

static void enter_state(comm_dispatch_state_t state, uint32_t timeout);
static void exit_state();


uint8_t sub_state_index = 0;


xTimeOutType time_out_definition;
portTickType max_wait_millis;

static void set_timeout(uint32_t millis)
{
	max_wait_millis = millis / portTICK_RATE_MS;

	/* Remember the time on entry. */
	vTaskSetTimeOutState(&time_out_definition);

}

static bool timeout(void)
{
	bool timeout = false;

	if (xTaskCheckForTimeOut(&time_out_definition, &max_wait_millis) == pdTRUE)
	{
		printf("comm_timeout.\r\n");
		timeout = true;
	}

	return timeout;
}

static void enter_state(comm_dispatch_state_t state, uint32_t timeout)
{
	reset_rx_buffer();
	_state = state;
	_substate = COMM_DISPATCH_INVOKE;
}

static void exit_state(void)
{
	reset_rx_buffer();
	_state = COMM_SOCKET_RESUME;
	_substate = COMM_DISPATCH_INVOKE;
}

static void enter_substate(comm_dispatch_sub_state_t state)
{
	_substate = state;
}

sys_result  comm_dispatch(modem_socket_t * socket)
{

	sys_result result;

	// STEP 1 try to resume an existing connection
	// continue loop
	if(_state == COMM_SOCKET_RESUME) {

		if(_substate == COMM_DISPATCH_INVOKE) {
			printf("modem_socketresume...\r\n");

			modem_socketresume(socket);
			enter_substate(COMM_DISPTCH_WAITREPLY);
			set_timeout(DEFAULT_COMM_SOCKETRESUME_TIMEOUT);

			return SYS_OK;
		} else if(_substate == COMM_DISPTCH_WAITREPLY) {

			if(timeout()) {
				printf("comm socket resume timeout.\r\n");
				enter_state(COMM_SOCKET_OPEN, 0);
			}

			result = modem_handle_socketresume();

			if(result == SYS_AT_OK) {
				xSemaphoreGive(connect_signal);
				comm_set_state(COMM_IDLE);
//				enter_state(COMM_SOCKET_WRITE, 0);
			}
		}

		return result;
	}

	// STEP 2 if resume fails then open a new connection
	// continue loop
	if(_state == COMM_SOCKET_OPEN) {

		if(_substate == COMM_DISPATCH_INVOKE) {
			printf("modem_socketopen...\r\n");

//			modem_socketopen(socket, MODEM_DEFAULT_HTTPSERVER);
			modem_socketopen(socket);
			enter_substate(COMM_DISPTCH_WAITREPLY);
			set_timeout(DEFAULT_COMM_SOCKETOPEN_TIMEOUT);

			return SYS_OK;
		} else if(_substate == COMM_DISPTCH_WAITREPLY) {

			if(timeout()) {
				printf("comm socket open timeout.\r\n");
				comm_set_state(COMM_IDLE);
			}

			result = modem_handle_socketopen();

			if(result == SYS_AT_OK) {
				xSemaphoreGive(connect_signal);
				comm_set_state(COMM_IDLE);
//				enter_state(COMM_SOCKET_WRITE, 0);
			}
		}

		return result;
	}

	// STEP 3a write the buffer to the socket and await the response
	// continue loop
	if(_state == COMM_SOCKET_WRITE) {

		if(_substate == COMM_DISPATCH_INVOKE) {
			printf("modem_socketwrite...\r\n");

			char * data = MODEM_DEFAULT_HTTPREQUEST;

			memset(socket->data_buffer, '\0', SOCKET_BUFFER_LEN+1);
			socket->bytes_received = 0;
			modem_socketwrite(socket, data);

			enter_substate(COMM_DISPTCH_WAITREPLY);
			set_timeout(5000);

			return SYS_OK;
		} else if(_substate == COMM_DISPTCH_WAITREPLY) {

			// wait up to 5 seconds.
			if(timeout())
				enter_state(COMM_SOCKET_SUSPEND, 0);

			if(bytes_received > 0) {
				socket->bytes_received = modem_copy_buffer(socket->data_buffer);
				socket->handle_data(socket->data_buffer, socket->bytes_received);
			}
		}

		return result;
	}

	// STEP 4 suspend the connection and set comm state back to COMM_IDLE
	// break loop
	if(_state == COMM_SOCKET_SUSPEND) {

		if(_substate == COMM_DISPATCH_INVOKE) {
			printf("modem_suspend...\r\n");

			modem_socketsuspend(socket);
			enter_substate(COMM_DISPTCH_WAITREPLY);

			return SYS_OK;
		} else if(_substate == COMM_DISPTCH_WAITREPLY) {

			result = modem_handle_socketsuspend();

			if(result == SYS_AT_OK) {
				exit_state();
				comm_set_state(COMM_IDLE);
			} else if (result == SYS_ERR_AT_NOCARRIER) {
				exit_state();
				comm_set_state(COMM_IDLE);
			}else if (result == SYS_ERR_AT_TIMEOUT) {
				exit_state();
				comm_set_state(COMM_IDLE);
			}
		}

		return result;
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


	return result;
}





