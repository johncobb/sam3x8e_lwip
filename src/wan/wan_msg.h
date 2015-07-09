/*
 * wan_msg.h
 *
 *  Created on: Jul 7, 2015
 *      Author: jcobb
 */

#ifndef WAN_MSG_H_
#define WAN_MSG_H_

#include <stdint.h>


#define MSG_SIZE

typedef struct app_msg_t {
	uint8_t messageType;
	uint8_t nodeType;
	uint64_t extAddr;
	uint16_t shortAddr;
	uint64_t routerAddr;
	//uint32_t    softVersion;
	//uint32_t    channelMask;
	uint16_t panId;
	uint8_t workingChannel;
	uint16_t parentShortAddr;
	uint8_t lqi;
	int8_t rssi;
	uint8_t ackByte;
	int32_t battery;
	int32_t temperature;
	uint8_t cs;

} app_msg_t;


// for sending from base station to routers
// minimum requirements
typedef struct
{
	uint8_t command;
	uint16_t pan_id;
	uint16_t short_id;
	uint8_t message_length;
	uint8_t dummy_data; // used as place holder for payload
}cmd_send_header_t;
#endif /* WAN_MSG_H_ */
