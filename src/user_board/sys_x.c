/*
 * sys.c
 *
 *  Created on: Jul 1, 2015
 *      Author: jcobb
 */



#include <stdint.h>
#include <string.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "sysclk.h"
#include "sys.h"
#include "modem.h"
#include "freertos_usart_serial.h"
#include "sio.h"



void sys_foobar(void)
{

}

//void sys_init(void)
//{
//	_sys_jiffies_ = sys_now();
//}



u32_t sio_write(sio_fd_t fd, u8_t *data, u32_t len)
{

//	freertos_usart_if modem_usart;
//
//	modem_usart =  (freertos_usart_if) fd;

	portTickType max_block_time_ticks = 20 / portTICK_RATE_MS;


	status_code_t status = freertos_usart_write_packet(modem_usart, data, len, max_block_time_ticks);

	return status;
}

void sio_read_abort(sio_fd_t fd)
{
	sio_fd_t x;

}


u32_t sio_read(sio_fd_t fd, u8_t *data, u32_t len)
{
//	freertos_usart_if modem_usart;
//
//	modem_usart =  (freertos_usart_if) fd;

	portTickType max_block_time_ticks = 20 / portTICK_RATE_MS;

	uint32_t bytes_received;

	bytes_received = freertos_usart_serial_read_packet(modem_usart, data, len, max_block_time_ticks);

	return bytes_received;
}



