/*
 * comm_close.c
 *
 *  Created on: Aug 12, 2015
 *      Author: jcobb
 */

#include "modem_defs.h"
#include "telit_modem_api.h"
#include "comm.h"
#include "comm_close.h"

typedef enum
{
	COMM_CLOSE_CONNECTION = 0
}comm_close_state_t;

typedef enum
{
	COMM_CLOSE_INVOKE = 0,
	COMM_CLOSE_WAITREPLY = 1
}comm_close_sub_state_t;


sys_result  comm_close(modem_socket_t * socket)
{
	sys_result result;

	if(socket->state_handle.state == COMM_CLOSE_CONNECTION) {

		if(socket->state_handle.substate == COMM_CLOSE_INVOKE) {
			printf("socket(%d) close...\r\n", socket->socket_id);

			modem_socketclose(socket);
			socket_entersubstate(socket, COMM_CLOSE_WAITREPLY);
			socket_settimeout(socket, DEFAULT_COMM_SOCKETCLOSE_TIMEOUT);
			result = SYS_OK;

		} else if(socket->state_handle.substate  == COMM_CLOSE_WAITREPLY) {

			if(socket_timeout(socket)) {
				printf("socket(%d) close timeout.\r\n", socket->socket_id);
				comm_enterstate(socket, COMM_IDLE);
				socket_exitstate(socket);
				xSemaphoreGive(tcp_close_signal);
			}
			result = modem_handle_socketclose(socket);

			if(result == SYS_AT_OK) {
				socket->socket_status = SCK_CLOSED;
				comm_enterstate(socket, COMM_IDLE);
				socket_exitstate(socket);
				xSemaphoreGive(tcp_close_signal);
			} else if (result == SYS_ERR_AT_NOCARRIER) {
				comm_enterstate(socket, COMM_IDLE);
				socket_exitstate(socket);
				xSemaphoreGive(tcp_close_signal);
			}else if (result == SYS_ERR_AT_TIMEOUT) {
				comm_enterstate(socket, COMM_IDLE);
				socket_exitstate(socket);
				xSemaphoreGive(tcp_close_signal);
			}
		}

		return result;
	}

	return result;
}
