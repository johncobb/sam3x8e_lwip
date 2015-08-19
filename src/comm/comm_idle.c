/*
 * comm_idle.c
 *
 *  Created on: Jul 8, 2015
 *      Author: jcobb
 */


#include "modem_defs.h"
#include "modem.h"
#include "comm.h"
#include "socket.h"
#include "comm_idle.h"

typedef enum
{
	COMM_IDLE_SETTIMEOUT = 0,
	COMM_IDLE_WAITTIMEOUT = 1
}comm_idle_state_t;


sys_result  comm_idle(modem_socket_t * socket)
{
	sys_result result;

	if(socket->state_handle.state == COMM_IDLE_SETTIMEOUT) {

		printf("socket(%d) idle.\r\n", socket->socket_id);
		printf("wait %d sec.\r\n", DEFAULT_COMM_IDLE_TIMEOUT/1000);

		socket_enterstate(socket, COMM_IDLE_WAITTIMEOUT);
		socket_settimeout(socket, DEFAULT_COMM_IDLE_TIMEOUT);
		result = SYS_OK;

	} else if(socket->state_handle.state == COMM_IDLE_WAITTIMEOUT) {

		if(socket_timeout(socket)) {
			socket_enterstate(socket, COMM_IDLE_SETTIMEOUT);
			result = SYS_OK;
		}
	}

	return result;

}
