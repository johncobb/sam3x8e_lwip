
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
#define PIN_LED_0_IDX	PIO_PB27_IDX
#define PIN_LED_0       {1 << 27, PIOB, ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT}
#define PIN_LED_0_MASK  (1 << 27)
#define PIN_LED_0_PIO   PIOB
#define PIN_LED_0_ID    ID_PIOB
#define PIN_LED_0_TYPE  PIO_OUTPUT_0
#define PIN_LED_0_ATTR  PIO_DEFAULT



/*! B.25 MODEM ON/OFF.*/
#define MDM_ONOFF_IDX       	PIO_PB25_IDX
#define MDM_ONOFF_MASK  		(1 << 25)
#define MDM_ONOFF_PIO   		PIOB
#define MDM_ONOFF_ID    		ID_PIOB
#define MDM_ONOFF_TYPE  		PIO_OUTPUT_0
#define MDM_ONOFF_ATTR  		PIO_DEFAULT

/*! C.28 MODEM ENABLE.*/
#define MDM_ENABLE_IDX       	PIO_PC28_IDX
#define MDM_ENABLE_MASK  		(1 << 28)
#define MDM_ENABLE_PIO   		PIOC
#define MDM_ENABLE_ID    		ID_PIOC
#define MDM_ENABLE_TYPE  		PIO_OUTPUT_0
#define MDM_ENABLE_ATTR  		PIO_DEFAULT


/*! C.26 MODEM RESET.*/
#define MDM_RESET_IDX       	PIO_PC26_IDX
#define MDM_RESET_MASK  		(1 << 26)
#define MDM_RESET_PIO   		PIOC
#define MDM_RESET_ID    		ID_PIOC
#define MDM_RESET_TYPE  		PIO_OUTPUT_0
#define MDM_RESET_ATTR  		PIO_DEFAULT


/*! C.25 MODEM POWMON.*/
#define MDM_POWMON_IDX       	PIO_PC25_IDX
#define MDM_POWMON_MASK  		(1 << 25)
#define MDM_POWMON_PIO   		PIOC
#define MDM_POWMON_ID    		ID_PIOC
#define MDM_POWMON_TYPE  		PIO_OUTPUT_0
#define MDM_POWMON_ATTR  		PIO_DEFAULT


/*! B.14 Labeled pin 52 on Arduino DUE.*/
#define MDM_TESTPIN1_IDX			PIO_PB14_IDX
#define MDM_TESTPIN1_MASK  		(1 << 14)
#define MDM_TESTPIN1_PIO   		PIOB
#define MDM_TESTPIN1_ID    		ID_PIOB
#define MDM_TESTPIN1_TYPE  		PIO_INPUT
#define MDM_TESTPIN1_ATTR  		PIO_DEFAULT

/*! C.22 Labeled pin 8 on Arduino DUE MODEM TESTPIN.*/
#define MDM_TESTPIN2_IDX			PIO_PC22_IDX
#define MDM_TESTPIN2_MASK  		(1 << 22)
#define MDM_TESTPIN2_PIO   		PIOC
#define MDM_TESTPIN2_ID    		ID_PIOC
#define MDM_TESTPIN2_TYPE  		PIO_OUTPUT_0
#define MDM_TESTPIN2_ATTR  		PIO_DEFAULT









#endif  // _CPH_BOARD_H
