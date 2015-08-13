/*
 * comm_idle.h
 *
 *  Created on: Jul 8, 2015
 *      Author: jcobb
 */

#ifndef COMM_IDLE_H_
#define COMM_IDLE_H_

#include "socket.h"

#define DEFAULT_COMM_IDLE_TIMEOUT	1000

sys_result  comm_idle(modem_socket_t * socket);

#endif /* COMM_IDLE_H_ */
