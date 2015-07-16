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

sys_result comm_idle(void)
{
	printf("comm_idle\r\n");

	printf("wait 1 sec.\r\n");
	vTaskDelay(1000);

	//comm_set_state(COMM_DISPATCH);

	return SYS_OK;

}
