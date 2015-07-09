/*
 * comm_http.c
 *
 *  Created on: Jul 8, 2015
 *      Author: jcobb
 */

#include "modem_defs.h"
#include "telit_modem_api.h"
#include "comm.h"
#include "comm_http.h"


typedef enum
{
	HTTP_RESUME = 0,
	HTTP_OPEN,
	HTTP_SEND,
	HTTP_WAITREPLY,
	HTTP_SUSPEND,
	HTTP_CLOSE
}comm_http_state_t;

comm_http_state_t comm_http_state = HTTP_RESUME;

at_modem_cmd_t at_httpsocket_commands[] =
{
		// fnc_handler, timeout, retries, fnc_callback
		{modem_httpsocketopen, 		5000, 0, modem_handle_httpsocketopen},
		{modem_httpsocketsuspend, 	2500, 0, modem_handle_httpsocketsuspend},
		{modem_httpsocketresume, 	5000, 0, modem_handle_httpsocketresume},
		{modem_httpsocketclose, 	2500, 0, modem_handle_httpsocketclose},
		//{modem_httpsocketget, 		5000, 0, modem_handle_httpsocketget},
		{NULL, 0, 0, NULL}
};






at_modem_cmd_t* at_cmd = NULL;

void comm_http_enter(comm_state_t state);



// resume
// connect
// close
// reconnect * special case

sys_result comm_http(void)
{
	// RESUME
	if (comm_http_state == HTTP_RESUME) {

		modem_httpsocketresume();

		sys_result sys_status = modem_handle_httpsocketresume();

		// we successfully established a connection to the server
		// tell the comm manager to start sending
		if(sys_status == SYS_OK) {
			comm_set_state(COMM_SEND);
		} else {
			comm_http_state = HTTP_OPEN;
		}
	}

	if(comm_state == HTTP_OPEN) {
		modem_httpsocketopen();

		sys_result sys_status = modem_handle_httpsocketopen();

		if(sys_status == SYS_OK) {
			comm_set_state(COMM_SEND);
		}

	}

	if(comm_state == HTTP_CLOSE) {
		modem_httpsocketclose();

		sys_result sys_status = modem_handle_httpsocketclose();

		if(sys_status == SYS_OK) {
			comm_set_state(COMM_IDLE);
		}

	}

	if(comm_state == HTTP_SUSPEND) {
		modem_httpsocketsuspend();

		sys_result sys_status = modem_handle_httpsocketsuspend();

		if(sys_status == SYS_OK) {
			comm_set_state(COMM_LISTEN);
		}

	}
	return SYS_OK;
}




void comm_http_enter(comm_state_t state)
{
	comm_state = state;
	at_modem_cmd_t* at_cmd = &(at_httpsocket_commands[comm_state]);
}

