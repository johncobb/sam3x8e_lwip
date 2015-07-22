/*
 * comm_connect.c
 *
 *  Created on: Jul 8, 2015
 *      Author: jcobb
 */



#include "modem_defs.h"
#include "telit_modem_api.h"
#include "comm.h"
#include "comm_connect.h"



typedef enum
{
	COMM_CONNECT_OPEN = 0,
	COMM_CONNECT_SUSPEND,
	COMM_CONNECT_STATUS,
	COMM_CONNECT_RESUME,
	COMM_CONNECT_CLOSE
}comm_connect_state_t;

typedef enum
{
	COMM_CONNECT_INVOKE = 0,
	COMM_CONNECT_WAITREPLY = 1
}comm_connect_sub_state_t;



static comm_connect_state_t _state = COMM_CONNECT_STATUS;
static comm_connect_sub_state_t _substate = COMM_CONNECT_INVOKE;

static void set_timeout(uint32_t millis);
static bool timeout(void);

static void enter_state(comm_connect_state_t state, uint32_t timeout);
static void exit_state();


static xTimeOutType time_out_definition;
static portTickType max_wait_millis;

// macro level socket function
void comm_socket_connect(void) {
	enter_state(COMM_CONNECT_STATUS, 0);
	_substate = COMM_CONNECT_INVOKE;
}

// macro level socket function
void comm_socket_suspend(void) {
	enter_state(COMM_CONNECT_SUSPEND, 0);
	_substate = COMM_CONNECT_INVOKE;
}

// macro level socket function
void comm_socket_close(void) {
	enter_state(COMM_CONNECT_CLOSE, 0);
	_substate = COMM_CONNECT_INVOKE;
}

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
		timeout = true;
	}

	return timeout;
}

static void enter_state(comm_connect_state_t state, uint32_t timeout)
{
	reset_rx_buffer();
	_state = state;
	_substate = COMM_CONNECT_INVOKE;
}

static void exit_state(void)
{
	reset_rx_buffer();
	_state = COMM_CONNECT_STATUS;
	_substate = COMM_CONNECT_INVOKE;
}

static void enter_substate(comm_connect_sub_state_t state)
{
	_substate = state;
}

sys_result  comm_connect(modem_socket_t * socket)
{
	sys_result result;


	// STEP 0 query socket status
	// continue loop
	if(_state == COMM_CONNECT_STATUS) {

		if(_substate == COMM_CONNECT_INVOKE) {
			printf("comm socket status?\r\n");

			modem_socketstatus(socket);
			enter_substate(COMM_CONNECT_WAITREPLY);
			set_timeout(DEFAULT_COMM_SOCKETSTATUS_TIMEOUT);

			result = SYS_OK;
		} else if(_substate == COMM_CONNECT_WAITREPLY) {

			if(timeout()) {
				printf("comm socket status timeout.\r\n");
				enter_state(COMM_CONNECT_OPEN, 0);
			}

			result = modem_handle_socketstatus(socket);
			/*
			SOCKET_CLOSED = 0,
			SOCKET_ACTIVE_DATATRANSFER,
			SOCKET_SUSPENDED,
			SOCKET_SUSPENDED_PENDINGDATA,
			SOCKET_LISTENING,
			SOCKET_INCOMINGCONNECTION
			*/

			switch(socket->socket_status) {
				case SOCKET_CLOSED: {
					enter_state(COMM_CONNECT_OPEN, 0);
					printf("socket_closed.\r\n");
					break;
				}
				case SOCKET_SUSPENDED:
				case SOCKET_SUSPENDED_PENDINGDATA: {
					 printf("socket_suspended.\r\n");
					 enter_state(COMM_CONNECT_RESUME, 0);
					 break;
				}
			}

		}

		return result;
	}

	// STEP 1 try to resume an existing connection
	// continue loop
	if(_state == COMM_CONNECT_RESUME) {

		if(_substate == COMM_CONNECT_INVOKE) {
			printf("modem_socketresume...\r\n");

			modem_socketresume(socket);
			enter_substate(COMM_CONNECT_WAITREPLY);
			set_timeout(DEFAULT_COMM_SOCKETRESUME_TIMEOUT);

			return SYS_OK;
		} else if(_substate == COMM_CONNECT_WAITREPLY) {

			if(timeout()) {
				printf("comm socket resume timeout.\r\n");
				enter_state(COMM_CONNECT_OPEN, 0);
			}

			result = modem_handle_socketresume();

			if(result == SYS_AT_OK) {
				xSemaphoreGive(connect_signal);
				comm_set_state(COMM_IDLE);
				exit_state();
//				enter_state(COMM_SOCKET_WRITE, 0);
			}
		}

		return result;
	}

	// STEP 2 if resume fails then open a new connection
	// continue loop
	if(_state == COMM_CONNECT_OPEN) {

		if(_substate == COMM_CONNECT_INVOKE) {
			printf("modem_socketopen...\r\n");

			modem_socketopen(socket);
			enter_substate(COMM_CONNECT_WAITREPLY);
			set_timeout(DEFAULT_COMM_SOCKETOPEN_TIMEOUT);

			return SYS_OK;
		} else if(_substate == COMM_CONNECT_WAITREPLY) {

			if(timeout()) {
				printf("comm socket open timeout.\r\n");
				comm_set_state(COMM_IDLE);
				exit_state();
			}

			result = modem_handle_socketopen();

			if(result == SYS_AT_OK) {
				// TODO: review options for state transition after connect

				// option 1: after establishing a connection suspend it
//				xSemaphoreGive(connect_signal);
//				comm_set_state(COMM_SUSPEND);
//				exit_state();

				// option 2: after establishing a connection return to idle
				xSemaphoreGive(connect_signal);
				comm_set_state(COMM_IDLE);
				exit_state();

				// option 3: after establishing a connection send a status
//				xSemaphoreGive(connect_signal);
//				comm_set_state(COMM_SEND);
//				exit_state();

			}
		}

		return result;
	}

	// STEP 3 suspend the connection and set comm state back to COMM_IDLE
	// break loop
	if(_state == COMM_CONNECT_SUSPEND) {

		if(_substate == COMM_CONNECT_INVOKE) {
			printf("modem_suspend...\r\n");

			modem_socketsuspend(socket);
			enter_substate(COMM_CONNECT_WAITREPLY);

			return SYS_OK;
		} else if(_substate == COMM_CONNECT_WAITREPLY) {

			result = modem_handle_socketsuspend();

			if(result == SYS_AT_OK) {
				comm_set_state(COMM_IDLE);
				exit_state();
			} else if (result == SYS_ERR_AT_NOCARRIER) {
				comm_set_state(COMM_IDLE);
				exit_state();
			}else if (result == SYS_ERR_AT_TIMEOUT) {
				comm_set_state(COMM_IDLE);
				exit_state();
			}
		}

		return result;
	}


}
