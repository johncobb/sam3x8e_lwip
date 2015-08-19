/*
 * comm.h
 *
 *  Created on: Jul 8, 2015
 *      Author: jcobb
 */

#ifndef COMM_H_
#define COMM_H_

#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "socket.h"
#include "comm_init.h"
#include "comm_config.h"
#include "comm_register.h"
#include "comm_idle.h"
#include "comm_connect.h"
#include "comm_send.h"
#include "comm_suspend.h"
#include "comm_close.h"
#include "comm_udp.h"
#include "comm_http.h"





//#define COMM_USE_MEMCPY	1

extern void comm_socket_connect(void);
extern void comm_socket_suspend(void);
extern void comm_socket_close();


xSemaphoreHandle comm_signal;

extern xSemaphoreHandle tcp_connect_signal;
extern xSemaphoreHandle tcp_send_signal;
extern xSemaphoreHandle tcp_suspend_signal;
extern xSemaphoreHandle tcp_close_signal;


#define DEFAULT_COMM_SOCKETOPEN_TIMEOUT			10000
#define DEFAULT_COMM_SOCKETRESUME_TIMEOUT		5000
#define DEFAULT_COMM_SOCKETSTATUS_TIMEOUT		500
#define DEFAULT_COMM_SOCKETSEND_TIMEOUT			1000
#define DEFAULT_COMM_SOCKETSUSPEND_TIMEOUT		5000
#define DEFAULT_COMM_SOCKETCLOSE_TIMEOUT		5000

#define QUEUE_TICKS		16
extern QueueHandle_t xCommQueue;
extern QueueHandle_t xCommQueueRequest;

#define COMM_BUFFER_LEN		256
extern uint8_t comm_buffer[];


#define	MAX_SOCKET_LEN		6

extern uint32_t bytes_received;

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
	COMM_CLOSE,
	COMM_WAITNEXTSTATE,
	COMM_WAITSOCKETREQUEST
}comm_state_t;

typedef enum
{
	COMM_INVOKE = 0,
	COMM_WAITREPLY = 1
}comm_sub_state_t;

typedef struct
{
	comm_state_t comm_state[MAX_SOCKET_LEN];


}comm_state_handler_t;

typedef enum
{
	FRAME_TCP = 0,
	FRAME_HTTP,
	FRAME_UDP
}comm_frame_type_t;

typedef enum
{
	REQUEST_CONNECT = 0,
	REQUEST_SEND,
	REQUEST_GET,
	REQUEST_CLOSE,
	REQUEST_ABORT,
	REQUEST_SUSPEND
}comm_request_type_t;


#define FRAME_BUFFER_LEN	256

typedef struct
{
	comm_frame_type_t type;			// FRAME_TCP, FRAME_HTTP, FRAME_UDP
	//socket_func_t handle_data;
	uint8_t buffer[FRAME_BUFFER_LEN+1];
	uint32_t len;

}comm_frame_t;


typedef struct
{
	comm_request_type_t type;
	uint8_t endpoint[SOCKET_IPENDPOINT_LEN+1];

}comm_request_t;



extern volatile bool comm_ready;
extern comm_state_t comm_state;

void comm_enterstate(modem_socket_t *socket, comm_state_t state);
void comm_set_state(comm_state_t state);


void create_comm_task(uint16_t stack_depth_words, unsigned portBASE_TYPE task_priority);

#endif /* COMM_H_ */
