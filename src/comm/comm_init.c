/*
 * comm_init.c
 *
 *  Created on: Jul 8, 2015
 *      Author: jcobb
 */

#include "modem_defs.h"
#include "modem.h"
#include "comm.h"
#include "comm_init.h"

sys_result comm_init(void)
{

	printf("comm_init\r\n");

	sys_result result = modem_init();

	if(result == SYS_OK) {
		comm_set_state(COMM_CONFIG);
	}

	return result;
}
