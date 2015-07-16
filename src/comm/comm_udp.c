/*
 * comm_udp.c
 *
 *  Created on: Jul 8, 2015
 *      Author: jcobb
 */
#include "modem_defs.h"
#include "telit_modem_api.h"
#include "comm_udp.h"


at_modem_cmd_t at_udpsocket_commands[] =
{
		// fnc_handler, timeout, retries, fnc_callback
		{modem_udpsocketopen, 		5000, 0, modem_handle_udpsocketopen},
		{modem_udpsocketsuspend, 	2500, 0, modem_handle_udpsocketsuspend},
		{modem_udpsocketresume, 	5000, 0, modem_handle_udpsocketresume},
		{modem_udpsocketclose, 		2500, 0, modem_handle_udpsocketclose},
		{NULL, 0, 0, NULL}
};


uint8_t modem_udpsocket(void)
{
	uint8_t socket_state_index = 0;
	uint8_t retries = 0;


	char * ptr = NULL;
	//sys_result sys_status;

	printf("opening udp socket...\r\n");

	while (true)
	{

		at_modem_cmd_t* at_cmd = &(at_udpsocket_commands[socket_state_index]);

		if(socket_state_index == 0) {
			at_cmd->fnc_handler();

			// TODO: HACK
			//sys_result sys_status = at_cmd->fnc_callback();

			sys_result sys_status;
			if(sys_status == SYS_AT_OK) {
				if(socket_state_index == 3)
					break;
			}

		}
		vTaskDelay(500);
	}

	return SYS_OK;
}
