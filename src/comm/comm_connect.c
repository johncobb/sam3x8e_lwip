/*
 * comm_connect.c
 *
 *  Created on: Jul 8, 2015
 *      Author: jcobb
 */



#include "modem_defs.h"
#include "telit_modem_api.h"
#include "comm_connect.h"


at_modem_cmd_t at_connect_commands[] =
{
		{NULL, 0, 0, NULL},
};


sys_result  comm_connect(void)
{
	return SYS_OK;

}
