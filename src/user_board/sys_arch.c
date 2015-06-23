/*
 * sys_arch.c
 *
 *  Created on: Jun 23, 2015
 *      Author: jcobb
 */

#include <stdint.h>
#include <string.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "sysclk.h"
#include "sys_arch.h"




volatile u32_t _sys_jiffies_ = 0;

void sys_init(void)
{
	_sys_jiffies_ = sys_now();
}

u32_t sys_now(void)
{
	return (u32_t) xTaskGetTickCount();
}

u32_t sio_write(sio_fd_t fd, u8_t *data, u32_t len)
{
	return 0;
}

u32_t sys_jiffies(void)
{
	return _sys_jiffies_;
}
