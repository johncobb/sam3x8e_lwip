/*
 * comm_config.c
 *
 *  Created on: Jul 8, 2015
 *      Author: jcobb
 */
#include "modem_defs.h"
#include "modem.h"
#include "comm.h"
#include "comm_init.h"

sys_result comm_config(void)
{
	printf("comm_config\r\n");

	sys_result result = modem_config();

	if(result == SYS_OK) {
		comm_set_state(COMM_REGISTER);
	}

	return result;

}
