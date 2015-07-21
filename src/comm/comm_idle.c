/*
 * comm_idle.c
 *
 *  Created on: Jul 8, 2015
 *      Author: jcobb
 */


#include "modem_defs.h"
#include "modem.h"
#include "comm.h"
#include "comm_idle.h"

typedef enum
{
	COMM_IDLE_SETTIMEOUT = 0,
	COMM_IDLE_WAITTIMEOUT = 1
}comm_idle_state_t;

comm_idle_state_t _state = COMM_IDLE_SETTIMEOUT;



static xTimeOutType time_out_definition;
static portTickType max_wait_millis;

// prototypes
static void set_timeout(uint32_t millis);
static bool timeout(void);

sys_result comm_idle(void)
{

	if(_state == COMM_IDLE_SETTIMEOUT) {

		printf("comm_idle\r\n");
		printf("wait %d sec.\r\n", DEFAULT_COMM_IDLE_TIMEOUT/1000);
		_state = COMM_IDLE_WAITTIMEOUT;
		set_timeout(DEFAULT_COMM_IDLE_TIMEOUT);
	} else if(COMM_IDLE_WAITTIMEOUT) {

		if(timeout()) {
			_state = COMM_IDLE_SETTIMEOUT;
			//comm_set_state(COMM_DISPATCH);
		}
	}
//	printf("comm_idle\r\n");
//
//	printf("wait 1 sec.\r\n");
//	vTaskDelay(1000);

	//comm_set_state(COMM_DISPATCH);

	return SYS_OK;

}

static void set_timeout(uint32_t millis)
{
	max_wait_millis = millis / portTICK_RATE_MS;

	/* Remember the time on entry. */
	vTaskSetTimeOutState(&time_out_definition);

}

static bool timeout(void)
{
	bool timeout = false;

	if (xTaskCheckForTimeOut(&time_out_definition, &max_wait_millis) == pdTRUE)
	{
//		printf("idle_timeout.\r\n");
		timeout = true;
	}

	return timeout;
}
