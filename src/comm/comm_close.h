/*
 * comm_close.h
 *
 *  Created on: Aug 12, 2015
 *      Author: jcobb
 */

#ifndef COMM_CLOSE_H_
#define COMM_CLOSE_H_

#include "socket.h"

#define DEFAULT_COMM_CLOSE_TIMEOUT	1000

sys_result  comm_close(modem_socket_t * socket);

#endif /* COMM_CLOSE_H_ */
