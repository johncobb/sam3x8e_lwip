/*
 * telit_modem_driver.c
 *
 *  Created on: Jul 7, 2015
 *      Author: jcobb
 */
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

void modem_skipesc(void)
{
	SEND_AT(MODEM_CMD_SKIPESC);
}

void modem_socketconfig(void)
{
	SEND_AT(MODEM_CMD_SOCKETCFG);
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

	sys_status = handle_result(MODEM_TOKEN_SGACT, &ptr, 5000);

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

	sys_status = handle_result(MODEM_TOKEN_SGACT, &ptr, 1000);

	// example result: #SGACT: 1,0
	if(sys_status == SYS_AT_OK) {
		printf("buffer:%s\r\n", ptr);
		ptr+=10;
		modem_status.context = ((ptr[0]-'0'));
		//printf("creg: %d\r\n", modem_status.creg);
	}

	return sys_status;

}

sys_result modem_handle_querynetwork(void)
{
	char * ptr = NULL;
	sys_result sys_status;

	sys_status = handle_result(MODEM_TOKEN_CREG, &ptr, 1000);

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

	sys_status = handle_result(MODEM_TOKEN_CSQ, &ptr, 1000);

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


sys_result modem_handle_udpsocketopen(void)
{
	char * ptr = NULL;
	sys_result sys_status;

	sys_status = handle_result(MODEM_TOKEN_CONNECT, &ptr, 5000);

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

	sys_status = handle_result(MODEM_TOKEN_OK, &ptr, 2500);

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

	sys_status = handle_result(MODEM_TOKEN_CONNECT, &ptr, 5000);

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

	sys_status = handle_result(MODEM_TOKEN_OK, &ptr, 2500);

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

	sys_status = handle_result(MODEM_TOKEN_CONNECT, &ptr, 5000);

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

	sys_status = handle_result(MODEM_TOKEN_OK, &ptr, 2500);

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

	sys_status = handle_result(MODEM_TOKEN_CONNECT, &ptr, 5000);

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

	sys_status = handle_result(MODEM_TOKEN_OK, &ptr, 2500);

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

	sys_status = handle_result(MODEM_TOKEN_HTTPOK, &ptr, 5000);

	printf("buffer:%s\r\n", ptr);

	if(sys_status == SYS_AT_OK) {
		modem_status.connection = CNX_OPENED;
		printf("socket open.\r\n");
	}

	return sys_status;
}
