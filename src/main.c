
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "asf.h"
#include "conf_board.h"
#include "cli_tasks.h"
#include "dialer.h"
#include "sysclk.h"
#include "ppp.h"
#include <stdio.h>
#include <string.h>




// folder structure modeled after FreeRTOS/Demo/CORTEX_ATSAM3X_Atmel_Studio
// source example: asf/thirdparty/freertos/demo/peripheral_control/sam3x8e_arduino_due_x


// Available commands
/*
 *
 *
help:
 Lists all the registered commands

task-stats:
 Displays a table showing the state of each FreeRTOS task

run-time-stats:
 Displays a table showing how much processing time each FreeRTOS task has used

echo-3-parameters <param1> <param2> <param3>:
 Expects three parameters, echos each in turn

echo-parameters <...>:
 Take variable number of parameters, echos each in turn

create-task <param>: *** NOT WORKING ***
 Creates a new task that periodically writes the parameter to the CLI output

delete-task:
 Deletes the task created by the create-task command
 *
 *
 */





/* The priorities at which various tasks will get created. */
#define mainUART_CLI_TASK_PRIORITY              (tskIDLE_PRIORITY + 1)
#define mainLED_TASK_PRIORITY					(tskIDLE_PRIORITY + 1)
#define mainDIALER_TASK_PRIORITY				(tskIDLE_PRIORITY + 1)

/* The stack sizes allocated to the various tasks. */
#define mainUART_CLI_TASK_STACK_SIZE    		(configMINIMAL_STACK_SIZE * 2)
#define mainLED_TASK_STACK_SIZE					(configMINIMAL_STACK_SIZE * 2)
#define mainDIALER_TASK_STACK_SIZE				(1024)


// TODO: REVIEW STACK SIZE ALLOCATION
//#define TASK_MONITOR_STACK_SIZE            (2048/sizeof(portSTACK_TYPE))
//#define TASK_MONITOR_STACK_PRIORITY        (tskIDLE_PRIORITY)
//#define TASK_LED_STACK_SIZE                (1024/sizeof(portSTACK_TYPE))
//#define TASK_LED_STACK_PRIORITY            (tskIDLE_PRIORITY)
//#define TASK_CLI_STACK_SIZE                (2048/sizeof(portSTACK_TYPE))
//#define TASK_CLI_STACK_PRIORITY            (tskIDLE_PRIORITY)


void create_led_task(void);



extern void vApplicationMallocFailedHook(void) {
	taskDISABLE_INTERRUPTS();
	for (;;)
		;
}

extern void vApplicationStackOverflowHook(xTaskHandle *pxTask, signed char *pcTaskName) {
	printf("stack overflow %x %s\r\n", pxTask, (portCHAR *) pcTaskName);
	for (;;)
		;
}

extern void vApplicationIdleHook(void) {
}

extern void vApplicationTickHook(void) {
}

bool pin_powmon = false;

//uint32_t timeout = 0;

static void task_led(void *pvParameters) {
	UNUSED(pvParameters);

//	timeout = ((uint32_t)sys_now()) + (2000);

	for (;;) {

//		if(sys_now() >= timeout) {
//			printf("timeout\r\n");
//			timeout = ((uint32_t)sys_now()) + (2000);
//		}

//		pio_toggle_pin(MDM_ONOFF_IDX);
//		pio_toggle_pin(MDM_ENABLE_IDX);
//		pio_toggle_pin(MDM_RESET_IDX);

		pio_toggle_pin(PIN_LED_0_IDX);
		pio_toggle_pin(PIN_LED1);
		pio_toggle_pin(PIN_LED2);
		vTaskDelay(500);
	}
}

#include "conf_uart_serial.h"
#include "stdio_serial.h"

#define USART_BAUDRATE      115200
#define USART_CHAR_LENGTH   US_MR_CHRL_8_BIT
#define USART_PARITY      US_MR_PAR_NO

static void configure_console(void) {

	sysclk_enable_peripheral_clock(PRINTF_USART_ID);

	//const usart_serial_options_t uart_serial_options = { .baudrate = CONF_UART_BAUDRATE, .paritytype = CONF_UART_PARITY, };

	const usart_serial_options_t uart_serial_options = {
	      .baudrate = USART_BAUDRATE,
	      .charlength =   USART_CHAR_LENGTH,
	      .paritytype = USART_PARITY,
	      .stopbits = false
	      //US_MR_CHMODE_NORMAL
	   };

	usart_serial_init(PRINTF_USART, &uart_serial_options);
	stdio_serial_init(PRINTF_USART, &uart_serial_options);

	usart_enable_tx(PRINTF_USART);
	usart_enable_rx(PRINTF_USART);
}

int main(void) {
	sysclk_init();
	board_init();

	configure_console();
	printf("CPH BaseStation v%d\r\n", 1);

	printf("create_uart_cli_task\r\n");
	create_uart_cli_task(CONSOLE_UART, mainUART_CLI_TASK_STACK_SIZE, mainUART_CLI_TASK_PRIORITY);
	printf("create_dialer_task\r\n");
	create_dialer_task(mainDIALER_TASK_STACK_SIZE, mainDIALER_TASK_PRIORITY);
	printf("create_led_task\r\n");
	create_led_task();


	printf("starting task scheduler\r\n");
	/* Start the scheduler. */
	vTaskStartScheduler();

	for (;;) {
	}

	/* Will only get here if there was insufficient memory to create the idle task. */
	return 0;
}

void create_led_task(void)
{
	/* Create task to make led blink */
	if (xTaskCreate(task_led, "Led", mainLED_TASK_STACK_SIZE, NULL, mainLED_TASK_PRIORITY, NULL) != pdPASS) {
		printf("Failed to create test led task\r\n");
	}
}

void assert_triggered(const char *file, uint32_t line)
{
	volatile uint32_t block_var = 0, line_in;
	const char *file_in;

	/* These assignments are made to prevent the compiler optimizing the
	values away. */
	file_in = file;
	line_in = line;
	(void) file_in;
	(void) line_in;

	taskENTER_CRITICAL();
	{
		while (block_var == 0) {
			/* Set block_var to a non-zero value in the debugger to
			step out of this function. */
		}
	}
	taskEXIT_CRITICAL();
}



