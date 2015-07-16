/*
 * comm.h
 *
 *  Created on: Jul 8, 2015
 *      Author: jcobb
 */

#ifndef COMM_H_
#define COMM_H_

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "comm_init.h"
#include "comm_config.h"
#include "comm_register.h"
#include "comm_idle.h"
#include "comm_dispatch.h"
#include "comm_connect.h"
#include "comm_udp.h"
#include "comm_http.h"

//#define COMM_USE_MEMCPY	1

xSemaphoreHandle comm_signal;



#define QUEUE_TICKS		16
extern QueueHandle_t xCommQueue;

#define COMM_BUFFER_LEN		256
extern uint8_t comm_buffer[];


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
	COMM_SUSPEND,
	COMM_DISPATCH
}comm_state_t;

typedef enum
{
	COMM_CMD_DISPATCH = 0,
	COMM_CMD_WAITREPLY = 1
}comm_cmd_state_t;

typedef enum
{
	FRAME_TCP = 0,
	FRAME_HTTP,
	FRAME_UDP
}comm_frame_type_t;


#define FRAME_BUFFER_LEN	256

typedef struct
{
	comm_frame_type_t type;			// FRAME_TCP, FRAME_HTTP, FRAME_UDP
	//socket_func_t handle_data;
	uint8_t buffer[FRAME_BUFFER_LEN+1];
	uint32_t len;

}comm_frame_t;


extern comm_state_t comm_state;

void comm_set_state(comm_state_t state);


void create_comm_task(uint16_t stack_depth_words, unsigned portBASE_TYPE task_priority);

#endif /* COMM_H_ */
