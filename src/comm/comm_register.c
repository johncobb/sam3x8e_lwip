/*
 * comm_register.c
 *
 *  Created on: Jul 8, 2015
 *      Author: jcobb
 */


#include "modem_defs.h"
#include "modem.h"
#include "telit_modem_api.h"
#include "comm.h"
#include "comm_register.h"


typedef enum
{
	COMM_REGISTER_QUERYNETWORK = 0,
	COMM_REGISTER_QUERYCONTEXT
} comm_register_state_t;

typedef enum
{
	COMM_REGISTER_INVOKE = 0,
	COMM_REGISTER_WAITREPLY = 1
}comm_register_sub_state_t;

static comm_register_state_t _state = COMM_REGISTER_QUERYNETWORK;
static comm_register_sub_state_t _substate = COMM_REGISTER_INVOKE;

static xTimeOutType time_out_definition;
static portTickType max_wait_millis;
static uint8_t _retries = 0;

// prototypes
static void enter_state(comm_register_state_t state);
static void exit_state();
static void enter_substate(comm_register_sub_state_t state);
static void exit_substate();

static void set_timeout(uint32_t millis);
static bool timeout(void);



static void enter_state(comm_register_state_t state)
{
	_state = state;
	_substate = COMM_REGISTER_INVOKE;
	_retries = 0;
	reset_rx_buffer();
}

static void exit_state(void)
{
	exit_substate();
	_state = COMM_REGISTER_QUERYNETWORK;
	_substate = COMM_REGISTER_INVOKE;
	_retries = 0;

}

static void enter_substate(comm_register_sub_state_t state)
{
	_substate = state;
	_retries = 0;
}

static void exit_substate()
{
	_substate = COMM_REGISTER_INVOKE;
	_retries = 0;
}

at_modem_cmd_t * at_cmd;


sys_result  comm_register(void)
{
	sys_result result;


	if(_state == COMM_REGISTER_QUERYNETWORK) {

		if(_substate == COMM_REGISTER_INVOKE) {

			// dispatch the function
			modem_querynetwork();
			// reset the timeout
			//set_timeout(10000);

			// change the _substate so that we can await reply from modem
			enter_substate(COMM_REGISTER_WAITREPLY);
			result = SYS_OK;

		} else if(_substate == COMM_REGISTER_WAITREPLY) {

			// parse results
			result = modem_handle_querynetwork();

			// check to see if command was successful
			// if so proceed
			if(modem_status.creg == 1) {
				enter_state(COMM_REGISTER_QUERYCONTEXT);
				result =  SYS_OK;
			} else {
				// toggle between DISPATCH and WAITREPLY
				// effectively querying the modem for desired result
				enter_substate(COMM_REGISTER_INVOKE);
				result = SYS_OK;
			}

		}

	} else if(_state == COMM_REGISTER_QUERYCONTEXT) {

		if(_substate == COMM_REGISTER_INVOKE) {

			// dispatch the function
			modem_querycontext();
			// reset the timeout
			//set_timeout(10000);

			// change the _substate so that we can await reply from modem
			enter_substate(COMM_REGISTER_WAITREPLY);
			result = SYS_OK;

		} else if(_substate == COMM_REGISTER_WAITREPLY) {

			// parse results
			result = modem_handle_querycontext();

			// check to see if command was successful
			// if so proceed
			if(modem_status.context == 1) {
				//comm_set_state(COMM_IDLE);
				// TODO: IT ALL BEGINS HERE SO PASS IN THE FIRST SOCKET
				comm_ready = true;
//				comm_enterstate(_socket, COMM_IDLE);
				// TODO: new code
				// we have not had our first socket initialized so
				// pass in a null for the socket and tell the state
				// machine to await the first socket request
				comm_enterstate(NULL, COMM_WAITSOCKETREQUEST);
				result = SYS_OK;
			} else {
				// toggle between DISPATCH and WAITREPLY
				// effectively querying the modem for desired result
				enter_substate(COMM_REGISTER_INVOKE);
				result = SYS_OK;
			}
		}
	}

	vTaskDelay(500);
	return result;



//	uint8_t reg_state_index = 0;
//	uint8_t retries = 0;
//
//
//	char * ptr = NULL;
//	//sys_result sys_status;
//
//	printf("registering network");
//
//	while (true)
//	{
//
//		at_modem_cmd_t* at_cmd = &(at_register_commands[reg_state_index]);
//
//		at_cmd->fnc_handler();
//		// TODO: HACK
//		//sys_result sys_status = at_cmd->fnc_callback();
//
//		sys_result sys_status;
//
//		if(reg_state_index == 0) {
//			if(sys_status == SYS_AT_OK) {
//				if(modem_status.creg == 1) {
//					reg_state_index++;
//					continue;
//				}
//			}
//		}
//
//		if (reg_state_index == 1) {
//			if(sys_status == SYS_AT_OK) {
//				if(modem_status.context == 1) {
//					break;
//				}
//			}
//		}
//		vTaskDelay(500);
//	}
//
//	comm_set_state(COMM_IDLE);
//
//	return SYS_OK;

}
