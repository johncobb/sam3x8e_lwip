/*
 * http_handler.c
 *
 *  Created on: Jul 14, 2015
 *      Author: jcobb
 */

#include <stdio.h>
#include <string.h>

#include "comm.h"
#include "telit_modem_api.h"
#include "modem_defs.h"
#include "modem.h"
#include "http_handler.h"



uint16_t http_buffer_index = 0;
uint8_t http_buffer[HTTP_BUFFER_LEN+1] = {0};

BaseType_t result;

sys_result http_handle_data(uint8_t *data, uint32_t len)
{
	//memcpy(http_buffer, data, len);

//	printf("http_handle_data (%lu): %s\r\n", len, data);
	comm_frame_t frame;

	memset(frame.buffer, '\0', FRAME_BUFFER_LEN+1);

	frame.type = FRAME_HTTP;
	frame.len = len;

	memcpy(frame.buffer, data, len);



	//printf("frame: %s\r\n", frame.buffer);


	result = xQueueSendToBack( xCommQueue, &frame, (TickType_t)0);

	if(result == pdTRUE) {
		//printf("message queued.\r\n");
	} else {
		printf("failed to enqueue message\r\n");
	}
	//result = xQueueSendToBack( xCommQueue, http_buffer, 0);

	//memset(http_buffer, '\0', HTTP_BUFFER_LEN+1);

	return SYS_OK;

}

#ifdef COMM_USE_MEMCPY
sys_result http_handle_data(uint8_t *data, uint32_t len)
{
	comm_frame_t *frame;

	frame = (comm_frame_t*) malloc(sizeof(comm_frame_t));

	if(frame) {

		memset(frame, 0, sizeof(comm_frame_t));

		// special case for http since we use null terminated strings
		memset(frame->buffer, '\0', FRAME_BUFFER_LEN+1);

		frame->type = FRAME_HTTP;
		frame->len = len;

		memcpy(frame->buffer, data, len);

		result = xQueueSendToBack( xCommQueue, frame, 0);
		free(frame);

//		if(result == pdTRUE)  {
//			return SYS_OK;
//		}

		return SYS_OK;
	}
}
#endif



/*
sys_result http_handle_data(uint8_t *data, uint32_t len)
{


	comm_frame_t *frame;

	memset(frame, 0, sizeof(comm_frame_t));

	// special case for http since we use null terminated strings
	memset(frame->buffer, '\0', FRAME_BUFFER_LEN+1);

	frame->type = FRAME_HTTP;
	frame->len = len;

	memcpy(frame->buffer, data, len);

	result = xQueueSendToBack( xCommQueue, frame, 0);

//	if(result == pdTRUE)  {
//		return SYS_OK;
//	}
	return SYS_OK;

}
*/

/*
sys_result http_handle_data(uint8_t *data, uint32_t len)
{

	// make sure not to overrun the buffer
	if((http_buffer_index + len) > HTTP_BUFFER_LEN) {

		result = xQueueSendToBack( xCommQueue, http_buffer, 0);

//		if(result == pdTRUE)  {
//		}

		// reset the buffer
		memset(http_buffer, '\0', HTTP_BUFFER_LEN+1);
		http_buffer_index = 0;

	}

	// if starting at zero then just copy in
	if(http_buffer_index == 0) {
		memcpy(http_buffer, data, len -1);
	} else {
		memcpy(&http_buffer[http_buffer_index], data, len);
	}
	// set the index to the new value
	http_buffer_index += len;

	//printf("http_handle_data: %s\r\n", data);
	return SYS_OK;

}
*/

