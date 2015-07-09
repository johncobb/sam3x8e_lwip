/*
 * comm_register.c
 *
 *  Created on: Jul 8, 2015
 *      Author: jcobb
 */


#include "modem_defs.h"
#include "telit_modem_api.h"
#include "comm.h"
#include "comm_register.h"


at_modem_cmd_t at_register_commands[] =
{
		//{modem_querysignal,		0, 0, modem_handle_querysignal},
		//{modem_activatecontext, 0, 0, modem_handle_activatecontext},
		{modem_querynetwork, 0, 0, modem_handle_querynetwork},
		{modem_querycontext,	0, 0, modem_handle_querycontext},
		{NULL, 0, 0, NULL},
};


sys_result  comm_register(void)
{
	uint8_t reg_state_index = 0;
	uint8_t retries = 0;


	char * ptr = NULL;
	//sys_result sys_status;

	printf("registering network");

	while (true)
	{

		at_modem_cmd_t* at_cmd = &(at_register_commands[reg_state_index]);

		at_cmd->fnc_handler();
		sys_result sys_status = at_cmd->fnc_callback();

		if(reg_state_index == 0) {
			if(sys_status == SYS_AT_OK) {
				if(modem_status.creg == 1) {
					reg_state_index++;
					continue;
				}
			}
		}

		if (reg_state_index == 1) {
			if(sys_status == SYS_AT_OK) {
				if(modem_status.context == 1) {
					break;
				}
			}
		}
		vTaskDelay(500);
	}

	comm_set_state(COMM_IDLE);

	return SYS_OK;

}
