/*
 * socket.h
 *
 *  Created on: Aug 5, 2015
 *      Author: jcobb
 */

#ifndef SOCKET_H_
#define SOCKET_H_

#include <stdint.h>
#include "modem.h"
#include "comm_if.h"
#include "http_handler.h"


#define SOCKET_POOL_LEN				6
#define SOCKET_BUFFER_LEN			128
#define SOCKET_IPENDPOINT_LEN		256
#define SOCKET_IPENDPOINT			"bs.cphandheld.com"



typedef enum
{
	SOCKET_CLOSED = 0,
	SOCKET_ACTIVE_DATATRANSFER,
	SOCKET_SUSPENDED,
	SOCKET_SUSPENDED_PENDINGDATA,
	SOCKET_LISTENING,
	SOCKET_INCOMINGCONNECTION
}modem_socket_state_t;

typedef enum
{
	SCK_CLOSED = 0,
	SCK_CLOSING,
	SCK_OPENED,
	SCK_OPENING,
	SCK_SUSPENDED,
	SCK_SUSPENDING,
}socket_status_t;

typedef enum
{
	SCK_SUCCESS = 0,
	SCK_ERR_TIMEOUT,
	SCK_ERR_NOCARRIER
}socket_error_t;



typedef sys_result (*socket_func_t)(uint8_t *data, uint32_t len);


typedef struct
{
	modem_socket_type type; // SOCKET_TCP, SOCKET_UDP
	modem_socket_protocol protocol; // SOCKET_PROT_TCP, SOCKET_PROT_HTTP, SOCKET_PROT_UDP
	uint16_t port;

}modem_socket_conf_t;


typedef sys_result (*comm_task_t)(void *);

typedef struct
{
	uint8_t state;
	uint8_t substate;
	xTimeOutType timeout;
	portTickType max_wait_millis;
} socket_state_t;

// cnx_id, ctx_id, pkt_size, glb_timeout, cnx_timeout (tenths of second), tx_timeout (tenths of second)
typedef struct
{
	uint8_t socket_id;
	uint8_t cnx_id;			// connection id
	uint8_t ctx_id;			// context id
	uint16_t pkt_size;		// packet size
	uint16_t glb_timeout;	// global timeout
	uint16_t cnx_timeout;	// connection timeout
	uint16_t tx_timeout;	// transmit timeout
	modem_socket_state_t modem_socket_state;
	modem_socket_conf_t socket_conf;
	uint8_t endpoint[SOCKET_IPENDPOINT_LEN+1];
	uint8_t rx_buffer[SOCKET_BUFFER_LEN+1];
	socket_func_t handle_data;
	uint32_t bytes_received;
	comm_task_t task_handler;
	socket_state_t state_handle;
	socket_status_t socket_status;
	socket_error_t	socket_error;

}modem_socket_t;

typedef struct
{
	uint8_t endpoint[SOCKET_IPENDPOINT_LEN+1];
	modem_socket_t *socket;
	socket_func_t handler;
	uint32_t timeout;
	portTickType max_wait_millis;


} socket_connection_t;

extern modem_socket_t *_socket;
extern modem_socket_t modem_sockets[];

extern uint8_t _socket_pool_index;

uint8_t socket_reserve(socket_connection_t *cnx);
uint8_t socket_free();
void socket_newconnection(socket_connection_t *cnx, uint8_t *endpoint, uint32_t timeout);





void socket_settimeout(modem_socket_t * socket, uint32_t millis);
bool socket_timeout(modem_socket_t * socket);

void socket_enterstate(modem_socket_t * socket, uint8_t state);
void socket_entersubstate(modem_socket_t * socket, uint8_t substate);
void socket_exitstate(modem_socket_t * socket);


#endif /* SOCKET_H_ */
