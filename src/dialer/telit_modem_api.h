/*
 * telit_modem_driver.h
 *
 *  Created on: Jul 7, 2015
 *      Author: jcobb
 */

#ifndef TELIT_MODEM_DRIVER_H_
#define TELIT_MODEM_DRIVER_H_

#include <stdint.h>
#include "modem_defs.h"

void modem_factory(void);
void modem_echooff(void);
void modem_setinterface(void);
void modem_setmsgformat(void);
void modem_setband(void);
void modem_setcontext(void);
void modem_setuserid(void);
void modem_setpassword(void);
void modem_setguardtime();
void modem_skipesc(void);
void modem_socketconfig(void);
void modem_socketconfigex(modem_socket_t *socket);



void modem_querycontext(void);
void modem_autocontext(void);
void modem_activatecontext(void);
void modem_querynetwork(void);
void modem_mobileequiperr(void);
void modem_querysignal(void);


void modem_socketopen(modem_socket_t *socket);
void modem_socketsuspend(modem_socket_t *socket);
void modem_socketresume(modem_socket_t *socket);
void modem_socketclose(modem_socket_t *socket);
void modem_socketwrite(modem_socket_t *socket, uint8_t * cmd);
void modem_socketlisten(modem_socket_t *socket);
void modem_socketaccept(modem_socket_t *socket);
void modem_socketstatus(modem_socket_t *socket);


void modem_udpsocketopen(void);
void modem_udpsocketclose(void);
void modem_udpsocketresume(void);
void modem_udpsocketsuspend(void);
void modem_udpsocketsend(uint8_t * cmd);

void modem_httpsocketopen(void);
void modem_httpsocketget(void);
void modem_httpsocketclose(void);
void modem_httpsocketresume(void);
void modem_httpsocketsuspend(void);
//void modem_httpsocketsend(uint8_t * cmd);


uint8_t modem_handle_activatecontext(void);
uint8_t modem_handle_querycontext(void);
uint8_t modem_handle_querynetwork(void);
uint8_t modem_handle_querysignal(void);

sys_result modem_handle_socketopen(modem_socket_t *socket);
uint8_t modem_handle_socketclose(modem_socket_t *socket);
sys_result modem_handle_socketresume(modem_socket_t *socket);
uint8_t modem_handle_socketsuspend(modem_socket_t *socket);
//uint8_t modem_handle_socketwrite(char *token);
sys_result modem_handle_socketwrite(modem_socket_t *socket);
uint8_t modem_handle_socketstatus(modem_socket_t *socket);

uint8_t modem_handle_udpsocketopen(void);
uint8_t modem_handle_udpsocketclose(void);
uint8_t modem_handle_udpsocketresume(void);
uint8_t modem_handle_udpsocketsuspend(void);


uint8_t modem_handle_httpsocketopen(void);
uint8_t modem_handle_httpsocketget(void);
uint8_t modem_handle_httpsocketclose(void);
uint8_t modem_handle_httpsocketresume(void);
uint8_t modem_handle_httpsocketsuspend(void);

#endif /* TELIT_MODEM_DRIVER_H_ */
