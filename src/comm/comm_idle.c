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

	printf("wait 5 sec.\r\n");
	vTaskDelay(5000);

	sys_result result = SYS_OK;

	return result;

}
