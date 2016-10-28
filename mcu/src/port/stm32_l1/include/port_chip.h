/*
 * port_chip.h
 *
 *  Created on: 21.10.2016
 *      Author: ak
 */

#ifndef MCU_SRC_PORT_STM32_L1_INCLUDE_PORT_CHIP_H_
#define MCU_SRC_PORT_STM32_L1_INCLUDE_PORT_CHIP_H_

#define portTOTAL_HEAP_SIZE 1024*16
#define portBYTE_ALIGNMENT 1
#define portPOINTER_SIZE_TYPE uint32_t

#include "stm32l1xx_conf.h"
#include "stm32l1xx.h"
#include <stdbool.h>

void port_EnterCritical( void );
void port_ExitCritical( void );

void port_chipInit();
void port_chipDeinit();
bool port_isDirectApplicationLaunchProgrammed(void);
void port_programDirectApplicationLaunch(void);
bool port_checkFlashConfiguration(bool usePrintf);

#define configPRIO_BITS       		4        /* 15 priority levels */

/* The highest interrupt priority that can be used by any interrupt service
routine that makes calls to interrupt safe FreeRTOS API functions.  DO NOT CALL
INTERRUPT SAFE FREERTOS API FUNCTIONS FROM ANY INTERRUPT THAT HAS A HIGHER
PRIORITY THAN THIS! (higher priorities are lower numeric values. */
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY	5

/* Interrupt priorities used by the kernel port layer itself.  These are generic
to all Cortex-M ports, and do not rely on any particular library functions. */
#define configKERNEL_INTERRUPT_PRIORITY 		( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )
/* !!!! configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to zero !!!!
See http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html. */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY 	( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )



#define ISR_PRIORITY_SERIAL_DBG				configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY+2

typedef enum{rer_none,
	rer_resetPin,
	rer_powerOnReset,
	rer_independendWatchdog,
	rer_windowWatchdog,
	rer_softwareReset,
	rer_rtc,
	rer_wupin
	} resetReason_t;

	resetReason_t portTestResetSource(void);
#endif /* MCU_SRC_PORT_STM32_L1_INCLUDE_PORT_CHIP_H_ */
