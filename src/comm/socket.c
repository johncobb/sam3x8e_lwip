/*
 * socket.c
 *
 *  Created on: Aug 7, 2015
 *      Author: jcobb
 */

#include <string.h>
#include "comm.h"
#include "socket.h"


modem_socket_t *_socket;
uint8_t _socket_pool_index = -1;

uint8_t socket_reserve(socket_connection_t *cnx)
{
	if(++_socket_pool_index == (SOCKET_POOL_LEN-1)) {
		_socket_pool_index = 5;
		return 0;
	}

	printf("reserving socket: (%d)\r\n", _socket_pool_index);

	// store the socket
	cnx->socket = &modem_sockets[_socket_pool_index];
	// assign the data handler
	cnx->socket->handle_data = cnx->handler;
	cnx->socket->state_handle.state = 0;
	cnx->socket->state_handle.substate = 0;


	return _socket_pool_index;
}

uint8_t socket_free()
{
	if(--_socket_pool_index == -1) {
		_socket_pool_index = 0;
		return 0;
	}

	printf("freeing socket: (%d)\r\n", _socket_pool_index);
	modem_sockets[_socket_pool_index].handle_data = NULL;
	modem_sockets[_socket_pool_index].socket_error = 0;

	return _socket_pool_index;
}



void socket_newconnection(socket_connection_t *cnx, uint8_t *endpoint, uint32_t timeout)
{
	printf("socket_newconnection()\r\n");

	// reserve the next socket
	socket_reserve(cnx);
	// set the connection timeout
	cnx->timeout = timeout;

	// prepare the buffers with '\0'
	memset(cnx->endpoint, '\0', SOCKET_IPENDPOINT_LEN+1);
	memset(cnx->socket->endpoint, '\0', SOCKET_IPENDPOINT_LEN+1);

	// copy in the endpoint address
	memcpy(cnx->endpoint, endpoint, SOCKET_IPENDPOINT_LEN);
	memcpy(cnx->socket->endpoint, endpoint, SOCKET_IPENDPOINT_LEN);
}

void socket_settimeout(modem_socket_t * socket, uint32_t millis)
{
	socket->state_handle.max_wait_millis = millis / portTICK_RATE_MS;

	/* Remember the time on entry. */
	vTaskSetTimeOutState(&socket->state_handle.timeout);
}

bool socket_timeout(modem_socket_t * socket)
{
	bool timeout = false;

	if (xTaskCheckForTimeOut(&socket->state_handle.timeout, &socket->state_handle.max_wait_millis) == pdTRUE)
	{
//		printf("idle_timeout.\r\n");
		timeout = true;
	}

	return timeout;
}


void socket_enterstate(modem_socket_t * socket, uint8_t state)
{
	socket->state_handle.state = state;
	socket->state_handle.substate = 0;
	socket->bytes_received = 0;
	memset(socket->rx_buffer, '\0', sizeof(socket->rx_buffer));
}

void socket_entersubstate(modem_socket_t * socket, uint8_t substate)
{
	socket->state_handle.substate = substate;
}

void socket_exitstate(modem_socket_t * socket)
{
	socket->state_handle.state = 0;
	socket->state_handle.substate = 0;
	socket->bytes_received = 0;
	memset(socket->rx_buffer, '\0', sizeof(socket->rx_buffer));
}
