/*
 * comm_send.c
 *
 *  Created on: Jul 22, 2015
 *      Author: jcobb
 */

#include <string.h>
#include "socket.h"
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




sys_result  comm_send(modem_socket_t * socket)
{
	sys_result result;

//	printf("comm_send: state: %d substate %d\r\n", socket->state_handle.state, socket->state_handle.substate);

	if(socket->state_handle.state == COMM_SEND_TRANSFERDATA) {

		if(socket->state_handle.substate == COMM_SEND_INVOKE) {
			printf("socket(%d) write...\r\n", socket->socket_id);

			//char * data = MODEM_DEFAULT_HTTPREQUEST;

			char * data = "20000000,01,01,00000007802F6399,E16B,00000007802DE16B,1973,16,0001,0000,00C3,0000,0001,EE000000,470000EE\r";

			memset(socket->rx_buffer, '\0', SOCKET_BUFFER_LEN+1);
			socket->bytes_received = 0;
			modem_socketwrite(socket, data);

			socket_entersubstate(socket, COMM_SEND_WAITREPLY);
			socket_settimeout(socket, 5000);

			result = SYS_OK;
		} else if(socket->state_handle.substate == COMM_SEND_WAITREPLY) {

			// wait up to 5 seconds.
			if(socket_timeout(socket)) {
				socket->socket_error = SCK_ERR_TIMEOUT;

				printf("socket(%d) send timeout\r\n", socket->socket_id);

				// TODO: review for proper transition after send
				// option 1: after sending suspend the connection
				comm_enterstate(socket, COMM_IDLE);
				socket_exitstate(socket);
				xSemaphoreGive(tcp_send_signal);
				result = SYS_OK;

			}

			if(socket->bytes_received > 0) {
//				printf("bytes_received: %lu, %s\r\n", socket->bytes_received, socket->rx_buffer);
				socket->handle_data(socket->rx_buffer, socket->bytes_received);
				memset(socket->rx_buffer, '\0', SOCKET_BUFFER_LEN+1);
				xSemaphoreGive(tcp_send_signal);
			}
			result = SYS_OK;
		}
	}

	return result;
}
