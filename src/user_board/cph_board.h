
#ifndef _CPH_BOARD_H
#define _CPH_BOARD_H

#include "compiler.h"
#include "system_sam3x.h"
#include "exceptions.h"
#include "pio.h"
#include "pmc.h"


#define UNIT_TEST_YIELD		vTaskDelay(10); continue;

// special function to initialize usart after configuring pins
void board_init_modem_usart(void);

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

#define MODEM_USART			USART0
#define MODEM_USART_ID		ID_USART0

#define PINS_USART0_PIO		PIOA
#define PINS_USART0_ID		ID_PIOA
#define PINS_USART0_TYPE		PIO_PERIPH_A
#define PINS_USART0_MASK 	(PIO_PA10A_RXD0 | PIO_PA11A_TXD0 | PIO_PB26A_CTS0 | PIO_PB25A_RTS0)
#define PINS_USART0_ATTR		PIO_DEFAULT



#define PRINTF_USART		USART1
#define PRINTF_USART_ID		ID_USART1

#define PINS_USART1_PIO		PIOA
#define PINS_USART1_ID		ID_PIOA
#define PINS_USART1_TYPE	PIO_PERIPH_A
#define PINS_USART1_MASK 	(PIO_PA12A_RXD1 | PIO_PA13A_TXD1)// | PIO_PA15A_CTS1 | PIO_PA14A_RTS1)
#define PINS_USART1_ATTR	PIO_DEFAULT





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



/* Arduino DUE Pin  6*/
#define PIN_LED1       		PIO_PC24_IDX
#define PIN_LED1_MASK  		(1 << 24)
#define PIN_LED1_PIO   		PIOC
#define PIN_LED1_ID    		ID_PIOC
#define PIN_LED1_TYPE  		PIO_OUTPUT_0
#define PIN_LED1_ATTR  		PIO_DEFAULT

/* Arduino DUE Pin  7*/
#define PIN_LED2       		PIO_PC23_IDX
#define PIN_LED2_MASK  		(1 << 23)
#define PIN_LED2_PIO   		PIOC
#define PIN_LED2_ID    		ID_PIOC
#define PIN_LED2_TYPE  		PIO_OUTPUT_0
#define PIN_LED2_ATTR  		PIO_DEFAULT






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
//#define MDM_ENABLE_ATTR  		PIO_PULLUP


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
#define MDM_POWMON_TYPE  		PIO_INPUT
#define MDM_POWMON_ATTR			PIO_PULLUP



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



/*****************************************************************************/
/*CAMERA */
/*****************************************************************************/
//
//#define CAM_TWI_INSTANCETWI0
//#define CAM_TWI_ADDRESSÊÊÊ(0x42>>1)
//#define CAM_TWI_OPTS{ BOARD_MCK, 100000, CAM_TWI_ADDRESS }
//#define CAM_TWI_DEFAULT_PACKET{ {0,0,0}, 1, 0, 1, CAM_TWI_ADDRESS }
//
//#define CAM_VSYNC_IDXPIO_PA15_IDX
//#define CAM_EN_IDXPIO_PC10_IDX
//
//#define CAM_CAP_PIOPIOA
//
//#define CAM_CLK_IDPMC_PCK_0
//#define CAM_CLK_SRCGENCLK_PCK_SRC_PLLBCK
//#define CAM_CLK_PRESPMC_PCK_PRES_CLK_4
//
//#define PIN_CAM_CLK_IDXPIO_PB13_IDX
//#define PIN_CAM_CLK_FLAGSPIO_PERIPH_B
//
//#define PIN_CAM_VSYNC_IDXCAM_VSYNC_IDX
//#define PIN_CAM_VSYNC_FLAGS(PIO_INPUT | PIO_DEFAULT)
//#define PIN_CAM_VSYNC_IFLAGS(PIO_PULLUP | PIO_IT_AIME | PIO_IT_RE_OR_HL | PIO_IT_EDGE)
//
//#define PIN_CAM_EN_IDXCAM_EN_IDX
//#define PIN_CAM_EN_FLAGSPIO_OUTPUT_0









#endif  // _CPH_BOARD_H
