/*
 * tcpip.h
 *
 *  Created on: Jul 21, 2015
 *      Author: jcobb
 */

#ifndef TCPIP_H_
#define TCPIP_H_

#include "socket.h"







#define DEFAULT_TCIP_CONNECTTIMEOUT	10000
#define DEFAULT_TCIP_SENDTIMEOUT	1000

typedef enum
{
	SYS_TCP_OK = 0,
	SYS_ERR_TCP_FAIL,
	SYS_ERR_TCP_FAIL_REQUESTENQUEUE,
	SYS_ERR_TCP_TIMEOUT
}tcp_result;

typedef tcp_result (*tcp_func_t)(uint8_t *data, uint32_t len);
typedef void (*tcp_data_callback_func_t)(uint8_t *data, uint32_t len);

tcp_result tcp_init(tcp_func_t handle_data);

tcp_result tcp_input(void);
tcp_result tcp_output(void);

tcp_result cph_tcp_connect(socket_connection_t *cnx);
tcp_result cph_tcp_send(socket_connection_t *cnx, uint8_t *packet, socket_func_t handler);
tcp_result cph_tcp_suspend();
tcp_result cph_tcp_close();
tcp_result cph_tcp_listen(uint8_t *endpoint, uint32_t len);


#endif /* TCPIP_H_ */
