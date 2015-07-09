/*
 * comm.h
 *
 *  Created on: Jul 8, 2015
 *      Author: jcobb
 */

#ifndef COMM_H_
#define COMM_H_


#include "comm_init.h"
#include "comm_config.h"
#include "comm_register.h"
#include "comm_idle.h"
#include "comm_connect.h"
#include "comm_udp.h"
#include "comm_http.h"

xSemaphoreHandle comm_signal;




extern QueueHandle_t xCommQueue;

typedef enum
{
	COMM_INIT = 0,
	COMM_CONFIG,
	COMM_REGISTER,
	COMM_IDLE,
	COMM_LISTEN,
	COMM_RESET,
	COMM_CONNECT,
	COMM_SEND,
	COMM_SUSPEND
}comm_state_t;


extern comm_state_t comm_state;

void comm_set_state(comm_state_t state);


void create_comm_task(uint16_t stack_depth_words, unsigned portBASE_TYPE task_priority);

#endif /* COMM_H_ */
