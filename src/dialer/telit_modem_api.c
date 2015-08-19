/*
 * telit_modem_driver.c
 *
 *  Created on: Jul 7, 2015
 *      Author: jcobb
 */
#include <stdio.h>
#include <string.h>

#include "telit_modem_api.h"
#include "modem_defs.h"
#include "modem.h"
#include "wan_msg.h"



void modem_factory(void)
{
	SEND_AT(MODEM_CMD_ATZ);
}

void modem_echooff(void)
{
	SEND_AT(MODEM_CMD_ECHOOFF);
}

void modem_setinterface(void)
{
	SEND_AT(MODEM_CMD_SELINT);
}

void modem_setmsgformat(void)
{
	SEND_AT(MODEM_CMD_SELINT);
}

void modem_setband(void)
{
	SEND_AT(MODEM_CMD_SETBAND);
}

void modem_setcontext(void)
{
	SEND_AT(MODEM_CMD_SETCONTEXT);
}

void modem_setuserid(void)
{
	SEND_AT(MODEM_CMD_SETUSERID);
}

void modem_setpassword(void)
{
	SEND_AT(MODEM_CMD_SETPASSWORD);
}

void modem_setguardtime(void)
{
	SEND_AT(MODEM_CMD_SETGUARDTIME);
}

void modem_skipesc(void)
{
	SEND_AT(MODEM_CMD_SKIPESC);
}

void modem_socketconfig(void)
{
	SEND_AT(MODEM_CMD_SOCKETCFG);
}

void modem_socketconfigex(modem_socket_t *socket)
{
	memset(scratch_buffer, '\0', SCRATCH_BUFFER_LEN);

	sprintf(scratch_buffer, MODEM_CMD_SOCKETCFGEX, socket->cnx_id, socket->ctx_id, socket->pkt_size, socket->glb_timeout, socket->cnx_timeout, socket->tx_timeout);
	SEND_AT(scratch_buffer);
	printf(scratch_buffer);

}

void modem_querycontext(void)
{
	SEND_AT(MODEM_CMD_QUERYCONTEXT);
}

void modem_autocontext(void)
{
	SEND_AT(MODEM_CMD_AUTOCTX);
}

void modem_activatecontext(void)
{
	SEND_AT(MODEM_CMD_ACTIVATECONTEXT);
}

void modem_querynetwork(void)
{
	SEND_AT(MODEM_CMD_QUERYNETWORK);
}

void modem_querysignal(void)
{
	SEND_AT(MODEM_CMD_QUERYSIGNAL);
}

void modem_mobileequiperr(void)
{
	SEND_AT(MODEM_CMD_MOBILEEQUIPERR);
}

// modem socket functions: open, suspend, resume, close
//void modem_socketopen(modem_socket_t *socket)
//{
//	memset(scratch_buffer, '\0', SCRATCH_BUFFER_LEN);
//	// "AT#SD=1,0,80,\"www.google.com\"\r"
//	sprintf(scratch_buffer, MODEM_CMD_SOCKETOPEN, socket->cnx_id, socket->socket_conf.type, socket->socket_conf.port, MODEM_DEFAULT_HTTPSERVER);
//	SEND_AT(scratch_buffer);
//	printf(scratch_buffer);
//	printf("\r\n");
//}


void modem_socketopen(modem_socket_t *socket)
{
	memset(scratch_buffer, '\0', SCRATCH_BUFFER_LEN);
	// "AT#SD=1,0,80,\"www.google.com\"\r"
	sprintf(scratch_buffer, MODEM_CMD_SOCKETOPEN, socket->cnx_id, socket->socket_conf.type, socket->socket_conf.port, socket->endpoint);
	SEND_AT(scratch_buffer);
	printf(scratch_buffer);
	printf("\r\n");
}

void modem_socketsuspend(modem_socket_t *socket)
{
	// "+++"
	SEND_AT(MODEM_CMD_SOCKETSUSPEND);
}

void modem_socketwrite(modem_socket_t *socket, uint8_t * cmd)
{
	SEND_RAW(cmd);
}

void modem_socketresume(modem_socket_t *socket)
{
	memset(scratch_buffer, '\0', SCRATCH_BUFFER_LEN);
	// "AT#SO=1\r"
	sprintf(scratch_buffer, MODEM_CMD_SOCKETRESUME, socket->cnx_id);
	SEND_AT(scratch_buffer);
	printf(scratch_buffer);
}

void modem_socketclose(modem_socket_t *socket)
{

	memset(scratch_buffer, '\0', SCRATCH_BUFFER_LEN);
	// "AT#SH=1\r"
	sprintf(scratch_buffer, MODEM_CMD_SOCKETCLOSE, socket->cnx_id);
	SEND_AT(scratch_buffer);
	printf(scratch_buffer);
}

void modem_socketlisten(modem_socket_t *socket)
{
	memset(scratch_buffer, '\0', SCRATCH_BUFFER_LEN);
	// "AT#SL=1,0,8080"
	sprintf(scratch_buffer, MODEM_CMD_SOCKETLISTEN, socket->cnx_id, socket->socket_conf.type, socket->socket_conf.port);
	SEND_AT(scratch_buffer);
	printf(scratch_buffer);
}

void modem_socketaccept(modem_socket_t *socket)
{
	memset(scratch_buffer, '\0', SCRATCH_BUFFER_LEN);
	// "AT#SA=1\r"
	sprintf(scratch_buffer, MODEM_CMD_SOCKETACCEPT, socket->cnx_id);
	SEND_AT(scratch_buffer);
	printf(scratch_buffer);
}

void modem_socketstatus(modem_socket_t *socket)
{
	// "AT#SS"
	SEND_AT(MODEM_CMD_SOCKETSTATUS);
}

void modem_udpsocketopen(void)
{
	SEND_AT(MODEM_CMD_UDPSOCKETOPEN);
}

void modem_udpsocketclose(void)
{
	SEND_AT(MODEM_CMD_UDPSOCKETCLOSE);
}

void modem_udpsocketresume(void)
{
	SEND_AT(MODEM_CMD_UDPSOCKETRESUME);
}

void modem_udpsocketsuspend(void)
{
	SEND_AT(MODEM_CMD_UDPSOCKETSUSPEND);
}

void modem_udpsocketsend(uint8_t * cmd)
{
	SEND_RAW(cmd);
}

void modem_httpsocketopen(void)
{
	SEND_AT(MODEM_CMD_HTTPSOCKETOPEN);
}

void modem_httpsocketclose(void)
{
	SEND_AT(MODEM_CMD_HTTPSOCKETCLOSE);
}

void modem_httpsocketresume(void)
{
	SEND_AT(MODEM_CMD_HTTPSOCKETRESUME);
}

void modem_httpsocketsuspend(void)
{
	SEND_AT(MODEM_CMD_HTTPSOCKETSUSPEND);
}

void modem_httpsocketget(void)
{
	SEND_AT(MODEM_CMD_HTTPSOCKETGET);
}


sys_result modem_handle_activatecontext(void)
{
	char * ptr = NULL;
	sys_result sys_status;

	sys_status = handle_result(MODEM_TOKEN_SGACT, &ptr);

	printf("buffer:%s\r\n", ptr);

	// example result: #SGACT: 1,0
	if(sys_status == SYS_AT_OK) {
		printf("buffer:%s\r\n", ptr);
		ptr+=10;
		modem_status.context = ((ptr[0]-'0'));
		//printf("creg: %d\r\n", modem_status.creg);
	}

	return sys_status;

}


sys_result modem_handle_querycontext(void)
{
	char * ptr = NULL;
	sys_result sys_status;

	sys_status = handle_result(MODEM_TOKEN_SGACT, &ptr);

	// example result: #SGACT: 1,0
	if(sys_status == SYS_AT_OK) {
		printf(".");
		//printf("buffer:%s\r\n", ptr);
		ptr+=10;
		modem_status.context = ((ptr[0]-'0'));
		//printf("creg: %d\r\n", modem_status.creg);
		if(modem_status.context == 1) {
			printf("\r\ncontext active.\r\n");
		}
	}

	return sys_status;

}

sys_result modem_handle_querynetwork(void)
{
	char * ptr = NULL;
	sys_result sys_status;

	sys_status = handle_result(MODEM_TOKEN_CREG, &ptr);

	//			// Example Response "+CREG: 0,0" // Module not registered not searching
	//			// Example Response "+CREG: 0,1" // Module registered on network
	//			// Example Response "+CREG: 0,2" // Module searching for network
	//			// Example Response "+CREG: 0,3" // Module registration denied
	//			// Example Response "+CREG: 0,4" // Module state unknown
	//			// Example Response "+CREG: 0,5" // Module registered roaming
	if(sys_status == SYS_AT_OK) {
		printf(".");
		//printf("buffer:%s\r\n", ptr);
		ptr+=9;
		modem_status.creg = ((ptr[0]-'0'));
		//printf("creg: %d\r\n", modem_status.creg);
		if(modem_status.creg == 1) {
			printf("\r\nmodem registered.\r\n");
		}
	}

	return sys_status;

}

sys_result modem_handle_querysignal(void)
{
	char * ptr = NULL;
	sys_result sys_status;

	sys_status = handle_result(MODEM_TOKEN_CSQ, &ptr);

	printf("buffer:%s\r\n", ptr);

	// Example Response "+CSQ: 9,99"
	if(sys_status == SYS_AT_OK) {
		//printf("buffer:%s\r\n", ptr);
		ptr+=8;
		uint8_t csq = ((ptr[0] - '0')*10) + (ptr[1] - '0');

		modem_status.signal = csq;

		if ((csq >= 1) && (csq <= 31))
		{
			char * msg = NULL;
			if (csq <= 1)
				msg = "no signal\0";
			else if (csq >= 2 && csq <= 11)
				msg = "poor\0";
			else if (csq >= 12 && csq <= 24)
				msg = "good\0";
			else if (csq >= 25 && csq <= 31)
				msg = "excellent\0";

			printf("signal: %d %s\r\n", modem_status.signal, msg);
		}

	}

	return sys_status;

}

sys_result modem_handle_socketopen(modem_socket_t *socket)
{
	char * ptr = NULL;
	sys_result sys_status;

//	sys_status = handle_result(MODEM_TOKEN_CONNECT, &ptr);

	sys_status = handle_result_ex(socket->rx_buffer, MODEM_TOKEN_CONNECT, &ptr);

	printf("buffer:%s\r\n", ptr);

	if(sys_status == SYS_AT_OK) {
		modem_status.connection = CNX_OPENED;
		printf("socket open.\r\n");
	}

	return sys_status;
}

sys_result modem_handle_socketclose(modem_socket_t *socket)
{
	char * ptr = NULL;
	sys_result sys_status;

	sys_status = handle_result(MODEM_TOKEN_OK, &ptr);

	if(sys_status == SYS_AT_OK) {
		modem_status.connection = CNX_CLOSED;
		printf("socket closed.\r\n");
	}

	return sys_status;

}

sys_result modem_handle_socketresume(modem_socket_t *socket)
{
	char * ptr = NULL;
	sys_result sys_status;

//	sys_status = handle_result(MODEM_TOKEN_CONNECT, &ptr);
	sys_status = handle_result_ex(socket->rx_buffer, MODEM_TOKEN_CONNECT, &ptr);

	if(sys_status == SYS_AT_OK) {
		modem_status.connection = CNX_OPENED;
		printf("socket opened.\r\n");
	}

	return sys_status;

}

sys_result modem_handle_socketsuspend(modem_socket_t *socket)
{
	char * ptr = NULL;
	sys_result sys_status;

//	sys_status = handle_result(MODEM_TOKEN_OK, &ptr);
	sys_status = handle_result_ex(socket->rx_buffer, MODEM_TOKEN_OK, &ptr);

	if(sys_status == SYS_AT_OK) {
		modem_status.connection = CNX_SUSPENDED;
		printf("socket suspended.\r\n");
	}

	return sys_status;

}

//sys_result modem_handle_socketwrite(char * token)
//{
//	char * ptr = NULL;
//	sys_result sys_status;
//
//	// TODO: NEED TO REVISE BASED UPON PROTOCOL
//	sys_status = handle_result(token, &ptr, MODEM_DEFAULT_SOCKETWRITE_TIMEOUT);
//
//	printf("buffer:%s\r\n", ptr);
//
//	if(sys_status == SYS_AT_OK) {
//		printf("socket write.\r\n");
//	}
//
//	return sys_status;
//}

sys_result modem_handle_socketwrite(modem_socket_t *socket)
{


	socket->bytes_received = handle_stream(socket->rx_buffer, SOCKET_BUFFER_LEN, MODEM_DEFAULT_SOCKETWRITE_TIMEOUT);

	//printf("socket->bytes_received=%lu\r\n", socket->bytes_received);
	return SYS_OK;
}

uint8_t modem_handle_socketstatus(modem_socket_t *socket)
{

	// build the token we're looking for #SS:1 or #SS:2
	memset(scratch_buffer, '\0', SCRATCH_BUFFER_LEN);
	sprintf(scratch_buffer, MODEM_TOKEN_SOCKETSTATUS_ID, socket->cnx_id);

	char * ptr = NULL;
	sys_result sys_status;

//	sys_status = handle_result(scratch_buffer, &ptr);
	sys_status = handle_result_ex(socket->rx_buffer, scratch_buffer, &ptr);

	/*
	 * example result:
	#SS: 1,4,217.201.131.110,21
	#SS: 2,2,217.201.131.110,1033,194.185.15.73,10510
	#SS: 3,3,217.201.131.110,1034,194.185.15.73,10510
	#SS: 4,1,217.201.131.110,1035,194.185.15.73,10510
	#SS: 5,0
	#SS: 6,0
	 */

	// TODO: RECURSIVLEY PARSE FOR TOKEN MODEM_TOKEN_SOCKETSTATUS
	uint8_t socket_index = 0;

	if(sys_status == SYS_AT_OK) {
		printf("buffer:%s\r\n", ptr);
		ptr+=7;
		modem_sockets[socket_index].modem_socket_state = ((ptr[0]-'0'));

		//printf("creg: %d\r\n", modem_status.creg);
	}

	return sys_status;
}


sys_result modem_handle_udpsocketopen(void)
{
	char * ptr = NULL;
	sys_result sys_status;

	sys_status = handle_result(MODEM_TOKEN_CONNECT, &ptr);

	printf("buffer:%s\r\n", ptr);

	if(sys_status == SYS_AT_OK) {
		modem_status.connection = CNX_OPENED;
		printf("socket open.\r\n");
	}

	return sys_status;
}

sys_result modem_handle_udpsocketclose(void)
{
	char * ptr = NULL;
	sys_result sys_status;

	sys_status = handle_result(MODEM_TOKEN_OK, &ptr);

	if(sys_status == SYS_AT_OK) {
		modem_status.connection = CNX_CLOSED;
		printf("socket closed.\r\n");
	}

	return sys_status;

}

sys_result modem_handle_udpsocketresume(void)
{
	char * ptr = NULL;
	sys_result sys_status;

	sys_status = handle_result(MODEM_TOKEN_CONNECT, &ptr);

	if(sys_status == SYS_AT_OK) {
		modem_status.connection = CNX_OPENED;
		printf("socket opened.\r\n");
	}

	return sys_status;

}

sys_result modem_handle_udpsocketsuspend(void)
{
	char * ptr = NULL;
	sys_result sys_status;

	sys_status = handle_result(MODEM_TOKEN_OK, &ptr);

	if(sys_status == SYS_AT_OK) {
		modem_status.connection = CNX_SUSPENDED;
		printf("socket suspended.\r\n");
	}

	return sys_status;

}


sys_result modem_handle_httpsocketopen(void)
{
	char * ptr = NULL;
	sys_result sys_status;

	sys_status = handle_result(MODEM_TOKEN_CONNECT, &ptr);

	printf("buffer:%s\r\n", ptr);

	if(sys_status == SYS_AT_OK) {
		modem_status.connection = CNX_OPENED;
		printf("socket open.\r\n");
	}

	return sys_status;
}

sys_result modem_handle_httpsocketclose(void)
{
	char * ptr = NULL;
	sys_result sys_status;

	sys_status = handle_result(MODEM_TOKEN_OK, &ptr);

	if(sys_status == SYS_AT_OK) {
		modem_status.connection = CNX_CLOSED;
		printf("socket closed.\r\n");
	}

	return sys_status;

}

sys_result modem_handle_httpsocketresume(void)
{
	char * ptr = NULL;
	sys_result sys_status;

	sys_status = handle_result(MODEM_TOKEN_CONNECT, &ptr);

	if(sys_status == SYS_AT_OK) {
		modem_status.connection = CNX_OPENED;
		printf("socket opened.\r\n");
	}

	return sys_status;

}

sys_result modem_handle_httpsocketsuspend(void)
{
	char * ptr = NULL;
	sys_result sys_status;

	sys_status = handle_result(MODEM_TOKEN_OK, &ptr);

	if(sys_status == SYS_AT_OK) {
		modem_status.connection = CNX_SUSPENDED;
		printf("socket suspended.\r\n");
	}

	return sys_status;

}

sys_result modem_handle_httpsocketget(void)
{
	char * ptr = NULL;
	sys_result sys_status;

	sys_status = handle_result(MODEM_TOKEN_HTTPOK, &ptr);

	printf("buffer:%s\r\n", ptr);

	if(sys_status == SYS_AT_OK) {
		modem_status.connection = CNX_OPENED;
		printf("socket open.\r\n");
	}

	return sys_status;
}
