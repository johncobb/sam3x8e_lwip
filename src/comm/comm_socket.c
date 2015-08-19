/*
 * comm_socket.c
 *
 *  Created on: Aug 18, 2015
 *      Author: jcobb
 */
#include <string.h>
#include "socket.h"
#include "modem_defs.h"
#include "modem.h"
#include "telit_modem_api.h"
#include "comm.h"
#include "comm_socket.h"
#include "http_handler.h"

typedef enum
{
	COMM_SOCKET_OPEN = 0,
	COMM_SOCKET_RESUME,
	COMM_SOCKET_SEND,
	COMM_SOCKET_SUSPEND,
	COMM_SOCKET_CLOSE
}comm_socket_state_t;

typedef enum
{
	COMM_SOCKET_INVOKE = 0,
	COMM_SOCKET_WAITREPLY = 1
}comm_socket_sub_state_t;



sys_result  comm_socket(modem_socket_t * socket)
{

	sys_result result;

	if(socket->state_handle.state == COMM_SOCKET_OPEN) {

		if(socket->state_handle.substate == COMM_SOCKET_INVOKE) {

			comm_enterstate(socket, COMM_CONNECT);
			return SYS_OK;

		}

		if(socket->state_handle.substate == COMM_SOCKET_WAITREPLY) {
			return SYS_OK;

		}

	}


	return result;
}
