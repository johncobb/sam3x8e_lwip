/*
 * sys_arch.h
 *
 *  Created on: Jun 23, 2015
 *      Author: jcobb
 */

#ifndef SYS_ARCH_H_
#define SYS_ARCH_H_


#include "board.h"
#include "sio.h"


typedef uint8_t            u8_t;
typedef int8_t             s8_t;
typedef uint16_t           u16_t;
typedef int16_t            s16_t;
typedef uint32_t           u32_t;
typedef int32_t            s32_t;
typedef uintptr_t          mem_ptr_t;

void sys_init(void);
u32_t sys_now(void);
u32_t sio_write(sio_fd_t fd, u8_t *data, u32_t len);
u32_t sys_jiffies(void);


#endif /* SYS_ARCH_H_ */
