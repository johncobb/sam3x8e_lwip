/*
 * comm_send.c
 *
 *  Created on: Jul 22, 2015
 *      Author: jcobb
 */

#include <string.h>
#include "modem_defs.h"
#include "modem.h"
#include "telit_modem_api.h"
#include "comm.h"
#include "comm_send.h"
#include "http_handler.h"


typedef enum
{
	COMM_SEND_TRANSFERDATA = 0,
	COMM_SEND_ABORT_TRANSFERDATA
}comm_send_state_t;

typedef enum
{
	COMM_SEND_INVOKE = 0,
	COMM_SEND_WAITREPLY = 1
}comm_send_sub_state_t;



static comm_send_state_t _state = COMM_SEND_TRANSFERDATA;
static comm_send_sub_state_t _substate = COMM_SEND_INVOKE;


static void set_timeout(uint32_t millis);
static bool timeout(void);

static void enter_state(comm_send_state_t state, uint32_t timeout);
static void exit_state();


static xTimeOutType time_out_definition;
static portTickType max_wait_millis;

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

static void enter_state(comm_send_state_t state, uint32_t timeout)
{
	reset_rx_buffer();
	_state = state;
	_substate = COMM_SEND_INVOKE;
}

static void exit_state(void)
{
	reset_rx_buffer();
	_state = COMM_SEND_TRANSFERDATA;
	_substate = COMM_SEND_INVOKE;
}

static void enter_substate(comm_send_sub_state_t state)
{
	_substate = state;
}

sys_result  comm_send(modem_socket_t * socket)
{

	sys_result result;


	if(_state == COMM_SEND_TRANSFERDATA) {

		if(_substate == COMM_SEND_INVOKE) {
			printf("modem_socketwrite...\r\n");

			//char * data = MODEM_DEFAULT_HTTPREQUEST;

			char * data = "20000000,01,01,00000007802F6399,E16B,00000007802DE16B,1973,16,0001,0000,00C3,0000,0001,EE000000,470000EE\r";

			memset(socket->data_buffer, '\0', SOCKET_BUFFER_LEN+1);
			socket->bytes_received = 0;
			modem_socketwrite(socket, data);

			enter_substate(COMM_SEND_WAITREPLY);
			set_timeout(DEFAULT_COMM_SOCKETSEND_TIMEOUT);

			return SYS_OK;
		} else if(_substate == COMM_SEND_WAITREPLY) {

			// wait up to 5 seconds.
			if(timeout()) {
				printf("comm send timeout\r\n");

				// TODO: review for proper transition after send
				// option 1: after sending suspend the connection
				comm_set_state(COMM_SUSPEND);

				// option 2: after sending resume to idle leaving connectionopen
				//comm_set_state(COMM_IDLE);

				exit_state();
			}

			if(bytes_received > 0) {
				socket->bytes_received = modem_copy_buffer(socket->data_buffer);
				socket->handle_data(socket->data_buffer, socket->bytes_received);
			}
		}

		return result;
	}
}
