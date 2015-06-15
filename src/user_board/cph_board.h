
#ifndef _CPH_BOARD_H
#define _CPH_BOARD_H

#include "compiler.h"
#include "system_sam3x.h"
#include "exceptions.h"
#include "pio.h"

/** Board oscillator settings */
#define BOARD_FREQ_SLCK_XTAL        (32768U)
#define BOARD_FREQ_SLCK_BYPASS      (32768U)
#define BOARD_FREQ_MAINCK_XTAL      (12000000U)
#define BOARD_FREQ_MAINCK_BYPASS    (12000000U)

/** Master clock frequency */
#define BOARD_MCK                   CHIP_FREQ_CPU_MAX

/** board main clock xtal statup time */
#define BOARD_OSC_STARTUP_US   15625

/** Name of the board */
#define BOARD_NAME "CPH-BOARD"
/** Board definition */
#define cphBoard
/** Family definition (already defined) */
#define sam3x
/** Core definition */
#define cortexm3

/* ------------------------------------------------------------------------ */
/* UART                                                                     */
/* ------------------------------------------------------------------------ */
/*! UART pins (UTXD0 and URXD0) definitions, PA8,9. (labeled RX0->0 and TX0->1)*/
#define PINS_UART        (PIO_PA8A_URXD | PIO_PA9A_UTXD)
#define PINS_UART_FLAGS  (PIO_PERIPH_A | PIO_DEFAULT)

#define CONSOLE_UART        UART
#define CONSOLE_UART_ID     ID_UART

#define PINS_UART_PIO  PIOA
#define PINS_UART_ID   ID_PIOA
#define PINS_UART_TYPE PIO_PERIPH_A
#define PINS_UART_MASK (PIO_PA8A_URXD | PIO_PA9A_UTXD)
#define PINS_UART_ATTR PIO_DEFAULT


/**
 * \file
 * USART0
 * - \ref PIN_USART0_RXD
 * - \ref PIN_USART0_TXD
 */
/* ------------------------------------------------------------------------ */
/* USART0                                                                   */
/* ------------------------------------------------------------------------ */
/*! USART0 pin RX  (labeled RX1 19)*/
/*! USART0 pin TX  (labeled TX1 18) */
/*! USART0 pin CTS  (labeled 24) */
/*! USART0 pin RTS  (labeled 2) */

#define BOARD_USART			USART0
#define BOARD_USART_ID		ID_USART0

#define PINS_USART_PIO		PIOA
#define PINS_USART_ID		ID_PIOA
#define PINS_USART_TYPE		PIO_PERIPH_A
#define PINS_USART_MASK 	(PIO_PA10A_RXD0 | PIO_PA11A_TXD0 | PIO_PB26A_CTS0 | PIO_PB25A_RTS0)
#define PINS_USART_ATTR		PIO_DEFAULT




/*----------------------------------------------------------------------------*/
/*	LEDS																	  */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*	LEDS																	  */
/*----------------------------------------------------------------------------*/

//#define LED_STATUS_IDX		PIO_PB27_IDX
//
//#define PINS_LED0_PIO			PIOB
//#define PINS_LED0_TYPE		PIO_OUTPUT_0
//#define PINS_LED0_MASK		PIO_PB27
//#define PINS_LED0_ATTR		PIO_DEFAULT






/*! LED #0 "L" pin definition (ORANGE).*/
#define LED_0_NAME      "Orange_LED"
#define LED0_GPIO       (PIO_PB27_IDX)
#define LED0_FLAGS      (PIO_TYPE_PIO_OUTPUT_1 | PIO_DEFAULT)
#define LED0_ACTIVE_LEVEL 0

#define PIN_LED_0       {1 << 27, PIOB, ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT}
#define PIN_LED_0_MASK  (1 << 27)
#define PIN_LED_0_PIO   PIOB
#define PIN_LED_0_ID    ID_PIOB
#define PIN_LED_0_TYPE  PIO_OUTPUT_0
#define PIN_LED_0_ATTR  PIO_DEFAULT






#endif  // _CPH_BOARD_H
