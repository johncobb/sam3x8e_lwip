/**
 * \file
 *
 * \brief Arduino Due/X board init.
 *
 * Copyright (c) 2011 - 2014 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */
 /**
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "compiler.h"
#include "board.h"
#include "conf_board.h"


void board_init(void)
{
#ifndef CONF_BOARD_KEEP_WATCHDOG_AT_INIT
	/* Disable the watchdog */
	WDT->WDT_MR = WDT_MR_WDDIS;
#endif


#ifdef CONF_BOARD_MODEM_CONTROL
	//pio_configure(MDM_TESTPIN1_PIO, MDM_TESTPIN1_TYPE, MDM_TESTPIN1_MASK, MDM_TESTPIN1_ATTR);
	pio_configure(MDM_ONOFF_PIO, MDM_ONOFF_TYPE, MDM_ONOFF_MASK, MDM_ONOFF_ATTR);
	pio_configure(MDM_ENABLE_PIO, MDM_ENABLE_TYPE, MDM_ENABLE_MASK, MDM_ENABLE_ATTR);
	pio_configure(MDM_RESET_PIO, MDM_RESET_TYPE, MDM_RESET_MASK, MDM_RESET_ATTR);
	pio_configure(MDM_POWMON_PIO, MDM_POWMON_TYPE, MDM_POWMON_MASK, MDM_POWMON_ATTR);
#endif


#ifdef CONF_BOARD_LEDS
	//pio_configure(PINS_LED0_PIO, PINS_LED0_TYPE, PINS_LED0_MASK, PINS_LED0_ATTR);
	pio_configure(PIN_LED_0_PIO, PIN_LED_0_TYPE, PIN_LED_0_MASK, PIN_LED_0_ATTR);
#endif

#ifdef CONF_BOARD_UART_CONSOLE
	//pio_configure(PINS_UART0_PIO, PINS_UART0_TYPE, PINS_UART0_MASK, PINS_UART0_ATTR);
	pio_configure(PINS_UART_PIO, PINS_UART_TYPE, PINS_UART_MASK, PINS_UART_ATTR);
#endif


#ifdef CONF_BOARD_USART
	pio_configure(PINS_USART_PIO, PINS_USART_TYPE, PINS_USART_MASK, PINS_USART_ATTR);

#endif



}
