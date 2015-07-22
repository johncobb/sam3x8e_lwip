/*
 * comm_suspend.c
 *
 *  Created on: Jul 22, 2015
 *      Author: jcobb
 */

#include "modem_defs.h"
#include "telit_modem_api.h"
#include "comm.h"
#include "comm_suspend.h"


typedef enum
{
	COMM_SUSPEND_CONNECTION = 0
}comm_suspend_state_t;

typedef enum
{
	COMM_SUSPEND_INVOKE = 0,
	COMM_SUSPEND_WAITREPLY = 1
}comm_suspend_sub_state_t;

static comm_suspend_state_t _state = COMM_SUSPEND_CONNECTION;
static comm_suspend_sub_state_t _substate = COMM_SUSPEND_INVOKE;

static void set_timeout(uint32_t millis);
static bool timeout(void);

static void enter_state(comm_suspend_state_t state, uint32_t timeout);
static void exit_state();

static xTimeOutType time_out_definition;
static portTickType max_wait_millis;

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
		printf("comm_timeout.\r\n");
		timeout = true;
	}

	return timeout;
}

static void enter_state(comm_suspend_state_t state, uint32_t timeout)
{
	reset_rx_buffer();
	_state = state;
	_substate = COMM_SUSPEND_INVOKE;
}

static void exit_state(void)
{
	reset_rx_buffer();
	_state = COMM_SUSPEND_CONNECTION;
	_substate = COMM_SUSPEND_INVOKE;
}

static void enter_substate(comm_suspend_sub_state_t state)
{
	_substate = state;
}

sys_result  comm_suspend(modem_socket_t * socket)
{

	sys_result result;

	if(_state == COMM_SUSPEND_CONNECTION) {

		if(_substate == COMM_SUSPEND_INVOKE) {
			printf("socket suspend...\r\n");

			modem_socketsuspend(socket);
			enter_substate(COMM_SUSPEND_WAITREPLY);
			set_timeout(DEFAULT_COMM_SOCKETSUSPEND_TIMEOUT);

			result = SYS_OK;
		} else if(_substate == COMM_SUSPEND_WAITREPLY) {

			if(timeout()) {
				printf("socket suspend timeout.\r\n");
				comm_set_state(COMM_IDLE);
				exit_state();
			}
			result = modem_handle_socketsuspend();

			if(result == SYS_AT_OK) {
				comm_set_state(COMM_IDLE);
				exit_state();
			} else if (result == SYS_ERR_AT_NOCARRIER) {
				comm_set_state(COMM_IDLE);
				exit_state();
			}else if (result == SYS_ERR_AT_TIMEOUT) {
				comm_set_state(COMM_IDLE);
				exit_state();
			}
		}

		return result;
	}

	return result;
}
