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
	COMM_CONNECT_STATUS = 0,
	COMM_CONNECT_OPEN,
	COMM_CONNECT_SUSPEND,
	COMM_CONNECT_RESUME,
	COMM_CONNECT_CLOSE
}comm_connect_state_t;

typedef enum
{
	COMM_CONNECT_INVOKE = 0,
	COMM_CONNECT_WAITREPLY = 1
}comm_connect_sub_state_t;


sys_result  comm_connect(modem_socket_t * socket)
{
	sys_result result;


	// STEP 0 query socket status
	// continue loop
	if(socket->state_handle.state == COMM_CONNECT_STATUS) {

		if(socket->state_handle.substate == COMM_CONNECT_INVOKE) {
			printf("comm(%d) socket status?\r\n", socket->socket_id);

			modem_socketstatus(socket);
			socket_entersubstate(socket, COMM_CONNECT_WAITREPLY);
			socket_settimeout(socket, DEFAULT_COMM_SOCKETSTATUS_TIMEOUT);

			result = SYS_OK;
		} else if(socket->state_handle.substate == COMM_CONNECT_WAITREPLY) {

			if(socket_timeout(socket)) {
				printf("comm(%d) socket status timeout.\r\n", socket->socket_id);
				socket_enterstate(socket, COMM_CONNECT_OPEN);
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

			switch(socket->modem_socket_state) {
				case SOCKET_CLOSED: {
					socket_enterstate(socket, COMM_CONNECT_OPEN);
					printf("socket_closed.\r\n");
					break;
				}
				case SOCKET_SUSPENDED:
				case SOCKET_SUSPENDED_PENDINGDATA: {
					 printf("socket(%d) suspended.\r\n", socket->socket_id);
					 socket_enterstate(socket, COMM_CONNECT_RESUME);
					 break;
				}
			}

		}

		return result;
	}

	// STEP 1 try to resume an existing connection
	// continue loop
	if(socket->state_handle.state == COMM_CONNECT_RESUME) {

		if(socket->state_handle.substate == COMM_CONNECT_INVOKE) {
			printf("socket(%d) resume...\r\n", socket->socket_id);

			modem_socketresume(socket);
			socket_entersubstate(socket, COMM_CONNECT_WAITREPLY);
			socket_settimeout(socket, DEFAULT_COMM_SOCKETRESUME_TIMEOUT);

			return SYS_OK;
		} else if(socket->state_handle.substate == COMM_CONNECT_WAITREPLY) {

			if(socket_timeout(socket)) {
				printf("socket(%d) resume timeout.\r\n", socket->socket_id);
				socket_enterstate(socket, COMM_CONNECT_OPEN);
			}

			result = modem_handle_socketresume(socket);

			if(result == SYS_AT_OK) {
				// socket is open
				socket->socket_status = SCK_OPENED;
				comm_enterstate(socket, COMM_IDLE);
				socket_exitstate(socket);
				// TODO: review options for state transition after connect
				xSemaphoreGive(tcp_connect_signal);

			}
		}

		return result;
	}

	// STEP 2 if resume fails then open a new connection
	// continue loop
	if(socket->state_handle.state == COMM_CONNECT_OPEN) {

		if(socket->state_handle.substate == COMM_CONNECT_INVOKE) {
			printf("socket(%d) open...\r\n", socket->socket_id);

			modem_socketopen(socket);
			socket_entersubstate(socket, COMM_CONNECT_WAITREPLY);
			socket_settimeout(socket, DEFAULT_COMM_SOCKETOPEN_TIMEOUT);

			return SYS_OK;
		} else if(socket->state_handle.substate == COMM_CONNECT_WAITREPLY) {

			if(socket_timeout(socket)) {
				socket->socket_error = SCK_ERR_TIMEOUT;
				printf("socket(%d) open timeout.\r\n", socket->socket_id);
				comm_enterstate(socket, COMM_IDLE);
				socket_exitstate(socket);
				// TODO: review options for state transition after connect
				xSemaphoreGive(tcp_connect_signal);
			}

			result = modem_handle_socketopen(socket);

			if(result == SYS_AT_OK) {
				socket->socket_status = SCK_OPENED;
//				comm_enterstate(socket, COMM_IDLE);

//				comm_enterstate(socket, COMM_SUSPEND);
//				socket_exitstate(socket);

				socket_entersubstate(socket, COMM_CONNECT_SUSPEND);
				socket_settimeout(socket, DEFAULT_COMM_SOCKETSUSPEND_TIMEOUT);

				// TODO: review options for state transition after connect
				xSemaphoreGive(tcp_connect_signal);
			}
		}

		return result;
	}

	// STEP 3 suspend the connection and set comm state back to COMM_IDLE
	// break loop
	if(socket->state_handle.state == COMM_CONNECT_SUSPEND) {

		if(socket->state_handle.substate == COMM_CONNECT_INVOKE) {
			printf("socket(%d) suspend...\r\n", socket->socket_id);

			vTaskDelay(MODEM_DEFUALT_ESCAPEGUARD_TIMEOUT);
			modem_socketsuspend(socket);
			vTaskDelay(MODEM_DEFUALT_ESCAPEGUARD_TIMEOUT);
			socket_entersubstate(socket, COMM_CONNECT_WAITREPLY);

			return SYS_OK;
		} else if(socket->state_handle.substate == COMM_CONNECT_WAITREPLY) {

			result = modem_handle_socketsuspend(socket);

			if(result == SYS_AT_OK) {
				comm_enterstate(socket, COMM_IDLE);
				socket_exitstate(socket);
			} else if (result == SYS_ERR_AT_NOCARRIER) {
				comm_enterstate(socket, COMM_IDLE);
				socket_exitstate(socket);
			}else if (result == SYS_ERR_AT_TIMEOUT) {
				comm_enterstate(socket, COMM_IDLE);
				socket_exitstate(socket);
			}
		}

		return result;
	}

	return result;

}

//sys_result  comm_connect(modem_socket_t * socket)
//{
//	sys_result result;
//
//
//	// STEP 0 query socket status
//	// continue loop
//	if(_state == COMM_CONNECT_STATUS) {
//
//		if(_substate == COMM_CONNECT_INVOKE) {
//			printf("comm socket status?\r\n");
//
//			modem_socketstatus(socket);
//			enter_substate(COMM_CONNECT_WAITREPLY);
//			set_timeout(DEFAULT_COMM_SOCKETSTATUS_TIMEOUT);
//
//			result = SYS_OK;
//		} else if(_substate == COMM_CONNECT_WAITREPLY) {
//
//			if(timeout()) {
//				printf("comm socket status timeout.\r\n");
//				enter_state(COMM_CONNECT_OPEN, 0);
//			}
//
//			result = modem_handle_socketstatus(socket);
//			/*
//			SOCKET_CLOSED = 0,
//			SOCKET_ACTIVE_DATATRANSFER,
//			SOCKET_SUSPENDED,
//			SOCKET_SUSPENDED_PENDINGDATA,
//			SOCKET_LISTENING,
//			SOCKET_INCOMINGCONNECTION
//			*/
//
//			switch(socket->cnx_state) {
//				case SOCKET_CLOSED: {
//					enter_state(COMM_CONNECT_OPEN, 0);
//					printf("socket_closed.\r\n");
//					break;
//				}
//				case SOCKET_SUSPENDED:
//				case SOCKET_SUSPENDED_PENDINGDATA: {
//					 printf("socket_suspended.\r\n");
//					 enter_state(COMM_CONNECT_RESUME, 0);
//					 break;
//				}
//			}
//
//		}
//
//		return result;
//	}
//
//	// STEP 1 try to resume an existing connection
//	// continue loop
//	if(_state == COMM_CONNECT_RESUME) {
//
//		if(_substate == COMM_CONNECT_INVOKE) {
//			printf("modem_socketresume...\r\n");
//
//			modem_socketresume(socket);
//			enter_substate(COMM_CONNECT_WAITREPLY);
//			set_timeout(DEFAULT_COMM_SOCKETRESUME_TIMEOUT);
//
//			return SYS_OK;
//		} else if(_substate == COMM_CONNECT_WAITREPLY) {
//
//			if(timeout()) {
//				printf("comm socket resume timeout.\r\n");
//				enter_state(COMM_CONNECT_OPEN, 0);
//			}
//
//			result = modem_handle_socketresume();
//
//			if(result == SYS_AT_OK) {
//				xSemaphoreGive(connect_signal);
//				comm_set_state(COMM_IDLE);
//				exit_state();
////				enter_state(COMM_SOCKET_WRITE, 0);
//			}
//		}
//
//		return result;
//	}
//
//	// STEP 2 if resume fails then open a new connection
//	// continue loop
//	if(_state == COMM_CONNECT_OPEN) {
//
//		if(_substate == COMM_CONNECT_INVOKE) {
//			printf("modem_socketopen...\r\n");
//
//			modem_socketopen(socket);
//			enter_substate(COMM_CONNECT_WAITREPLY);
//			set_timeout(DEFAULT_COMM_SOCKETOPEN_TIMEOUT);
//
//			return SYS_OK;
//		} else if(_substate == COMM_CONNECT_WAITREPLY) {
//
//			if(timeout()) {
//				printf("comm socket open timeout.\r\n");
//				comm_set_state(COMM_IDLE);
//				exit_state();
//			}
//
//			result = modem_handle_socketopen();
//
//			if(result == SYS_AT_OK) {
//				// TODO: review options for state transition after connect
//
//				// option 1: after establishing a connection suspend it
////				xSemaphoreGive(connect_signal);
////				comm_set_state(COMM_SUSPEND);
////				exit_state();
//
//				// option 2: after establishing a connection return to idle
//				xSemaphoreGive(connect_signal);
//				comm_set_state(COMM_IDLE);
//				exit_state();
//
//				// option 3: after establishing a connection send a status
////				xSemaphoreGive(connect_signal);
////				comm_set_state(COMM_SEND);
////				exit_state();
//
//			}
//		}
//
//		return result;
//	}
//
//	// STEP 3 suspend the connection and set comm state back to COMM_IDLE
//	// break loop
//	if(_state == COMM_CONNECT_SUSPEND) {
//
//		if(_substate == COMM_CONNECT_INVOKE) {
//			printf("modem_suspend...\r\n");
//
//			modem_socketsuspend(socket);
//			enter_substate(COMM_CONNECT_WAITREPLY);
//
//			return SYS_OK;
//		} else if(_substate == COMM_CONNECT_WAITREPLY) {
//
//			result = modem_handle_socketsuspend();
//
//			if(result == SYS_AT_OK) {
//				comm_set_state(COMM_IDLE);
//				exit_state();
//			} else if (result == SYS_ERR_AT_NOCARRIER) {
//				comm_set_state(COMM_IDLE);
//				exit_state();
//			}else if (result == SYS_ERR_AT_TIMEOUT) {
//				comm_set_state(COMM_IDLE);
//				exit_state();
//			}
//		}
//
//		return result;
//	}
//
//	return result;
//
//
//}

