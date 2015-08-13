/*
 * comm_suspend.c
 *
 *  Created on: Jul 22, 2015
 *      Author: jcobb
 */

#include "modem_defs.h"
#include "telit_modem_api.h"
#include "comm.h"
#include "comm_suspend.h"


typedef enum
{
	COMM_SUSPEND_CONNECTION = 0
}comm_suspend_state_t;

typedef enum
{
	COMM_SUSPEND_INVOKE = 0,
	COMM_SUSPEND_WAITREPLY = 1
}comm_suspend_sub_state_t;


sys_result  comm_suspend(modem_socket_t * socket)
{
	sys_result result;

	if(socket->state_handle.state == COMM_SUSPEND_CONNECTION) {

		if(socket->state_handle.substate == COMM_SUSPEND_INVOKE) {
			printf("socket suspend...\r\n");

			modem_socketsuspend(socket);
			socket_entersubstate(socket, COMM_SUSPEND_WAITREPLY);
			socket_settimeout(socket, DEFAULT_COMM_SOCKETSUSPEND_TIMEOUT);

			result = SYS_OK;
		} else if(socket->state_handle.substate  == COMM_SUSPEND_WAITREPLY) {

			if(socket_timeout(socket)) {
				printf("socket suspend timeout.\r\n");
				comm_enterstate(socket, COMM_IDLE);
				socket_exitstate(socket);
				xSemaphoreGive(tcp_suspend_signal);
			}
			result = modem_handle_socketsuspend(socket);

			if(result == SYS_AT_OK) {
				socket->socket_status = SCK_SUSPENDED;
				comm_enterstate(socket, COMM_IDLE);
				socket_exitstate(socket);
				xSemaphoreGive(tcp_suspend_signal);
			} else if (result == SYS_ERR_AT_NOCARRIER) {
				comm_enterstate(socket, COMM_IDLE);
				socket_exitstate(socket);
				xSemaphoreGive(tcp_suspend_signal);
			}else if (result == SYS_ERR_AT_TIMEOUT) {
				comm_enterstate(socket, COMM_IDLE);
				socket_exitstate(socket);
				xSemaphoreGive(tcp_suspend_signal);
			}
		}

		return result;
	}

	return result;
}
