/*
 * comm_config.c
 *
 *  Created on: Jul 8, 2015
 *      Author: jcobb
 */
#include "modem_defs.h"
#include "modem.h"
#include "telit_modem_api.h"
#include "comm.h"
#include "comm_init.h"

typedef enum
{
	COMM_CONFIG_MODEM = 0,
	COMM_CONFIG_SOCKETS
} comm_config_state_t;

typedef enum
{
	COMM_CONFIG_INVOKE = 0,
	COMM_CONFIG_WAITREPLY = 1
}comm_config_sub_state_t;

static comm_config_state_t _state = COMM_CONFIG_MODEM;
static comm_config_sub_state_t _substate = COMM_CONFIG_INVOKE;


at_modem_cmd_t at_cfg_commands[] =
{
		// fnc_handler, timeout, retries, waitingreply
		{modem_factory, 		1000, 0, false},
		{modem_echooff, 		1000, 0, false},
		{modem_setinterface, 	1000, 0, false},
		{modem_setmsgformat, 	1000, 0, false},
		//{modem_setband, 		1000, 0, false},
		{modem_setcontext, 		1000, 0, false},
		{modem_setuserid, 		1000, 0, false},
		{modem_setpassword, 	1000, 0, false},
		{modem_setguardtime, 	1000, 0, false},
		{modem_skipesc, 		1000, 0, false},
		{modem_socketconfig, 	1000, 0, false},
		{modem_mobileequiperr, 	1000, 0, false},
		{NULL, 0, 0, NULL}
};

static uint8_t _config_index = 0;
static uint8_t _socket_index = 0;

static xTimeOutType time_out_definition;
static portTickType max_wait_millis;
static uint8_t _retries = 0;

// prototypes
static void enter_state(comm_config_state_t state);
static void exit_state();
static void enter_substate(comm_config_sub_state_t state);
static void exit_substate();

static void set_timeout(uint32_t millis);
static bool timeout(void);


static void enter_state(comm_config_state_t state)
{
	_state = state;
	_substate = COMM_CONFIG_INVOKE;
	_config_index = 0;
	_retries = 0;
}

static void exit_state(void)
{
	exit_substate();
	_state = COMM_CONFIG_MODEM;
	_substate = COMM_CONFIG_INVOKE;
	_config_index = 0;
	_retries = 0;
}

static void enter_substate(comm_config_sub_state_t state)
{
	_substate = state;
	_retries = 0;
}

static void exit_substate()
{
	_substate = COMM_CONFIG_INVOKE;
	_retries = 0;
}

static void next()
{
	reset_rx_buffer();
	_config_index++;
}

static void next_socket()
{
	reset_rx_buffer();
	_socket_index++;
}

at_modem_cmd_t * at_cmd;
modem_socket_t * socket;

sys_result comm_config(void)
{

	sys_result result;

	// state_index 0: process all config commands
	// state_index 1: process all socket commands

	if(_state == COMM_CONFIG_MODEM) {

		if(_substate == COMM_CONFIG_INVOKE) {
			at_cmd = &(at_cfg_commands[_config_index]);

			// check to see if we have reached the end of the array
			// if so then proceed to next comm state
			if(at_cmd->fnc_handler == NULL) {
				enter_state(COMM_CONFIG_SOCKETS);
				//return SYS_OK reporting current state was successful
				return SYS_OK;
			}

			// dispatch the function
			at_cmd->fnc_handler();
			// reset the timeout
			set_timeout(at_cmd->timeout);

			// change the _substate so that we can await reply from modem
			enter_substate(COMM_CONFIG_WAITREPLY);
			return SYS_OK;

		} else if(_substate == COMM_CONFIG_WAITREPLY) {

			// parse results
			result = modem_config_handler();

			// check to see if command was successful
			// if so proceed
			if(result == SYS_AT_OK) {
				// all is good so move to next config
				next();
				// reset substate back to dispatch
				enter_substate(COMM_CONFIG_INVOKE);
				return SYS_OK;
			}

			// check for timeout
//			if(timeout()) {
//				// check for retries
//				if(_retries > at_cmd->retries) {
//					printf("max retries reached for command.\r\n");
//					exit_state();
//					comm_set_state(COMM_INIT);
//					return SYS_ERR_FAIL;
//				}
//				_retries ++;
//				_substate = COMM_CONFIG_DISPATCH;
//				return SYS_ERR_AT_TIMEOUT;
//			}
		}

	} else if (_state == COMM_CONFIG_SOCKETS) {

		if(_substate == COMM_CONFIG_INVOKE) {

			socket = &(modem_sockets[_socket_index]);
			// check to see if we have reached the end of the array
			// if so then proceed to next state
			if(socket->cnx_id == 0) {

//				printf("config done.\r\n");
//				vTaskDelay(500);
//				return SYS_CONFIG_OK;

				exit_state();
				comm_set_state(COMM_REGISTER);
				//return SYS_OK reporting current state was successful
				return SYS_OK;
			}

			// dispatch the function
			modem_socketconfigex(socket);
			// reset the timeout
			set_timeout(1000);

			// change the _substate so that we can await reply from modem
			enter_substate(COMM_CONFIG_WAITREPLY);
			return SYS_OK;

		} else if(_substate == COMM_CONFIG_WAITREPLY) {

			// parse results
			result = modem_config_handler();

			// check to see if command was successful
			// if so proceed
			if(result == SYS_AT_OK) {
				// all is good so move to next config
				next_socket();
				// reset substate back to dispatch
				enter_substate(COMM_CONFIG_INVOKE);
				return SYS_OK;
			}
		}

	}

	return result;

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
		printf("comm_timeout.\r\n");
		timeout = true;
	}

	return timeout;
}
