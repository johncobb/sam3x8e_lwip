/*
 * http_handler.h
 *
 *  Created on: Jul 14, 2015
 *      Author: jcobb
 */

#ifndef HTTP_HANDLER_H_
#define HTTP_HANDLER_H_

#include "modem.h"

#define HTTP_BUFFER_LEN		1024
extern uint8_t http_buffer[];

sys_result http_handle_data(uint8_t *data, uint32_t len);

#endif /* HTTP_HANDLER_H_ */
