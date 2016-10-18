/*
 *    Copyright (C) 2015  Crystal-Photonics GmbH
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef MAIN_H_
#define MAIN_H_


#include "board.h"

#include "channel_codec/channel_codec_types.h"




#define mainLED_TASK_STACK				( ( unsigned short ) 550 )
#define mainKEY_TASK_STACK				( ( unsigned short ) 300 )
#define mainRPC_SERIAL_TASK_STACK		( ( unsigned short ) 300 )
#define mainADC_TASK_STACK				( ( unsigned short ) 300 )

#define mainRPC_TASK_SERIAL_PRIORITY	tskIDLE_PRIORITY + 1
#define mainLED_TASK_PRIORITY			tskIDLE_PRIORITY + 4
#define mainKEY_TASK_PRIORITY			tskIDLE_PRIORITY + 3
#define mainADC_TASK_PRIORITY			tskIDLE_PRIORITY + 3

#define ISR_PRIORITY_SERIAL_DBG				configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY+2
#define ISR_PRIORITY_PIO					configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY+0
#define ISR_PRIORITY_COUNTER				configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY+0
#define ISR_PRIORITY_ADC					configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY+0
#define ISR_PRIORITY_PIO_ADC				configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY-1


void sleepAboutMS(int ms);


typedef enum{ semTest=0, semCount} t_semaphoreHandle;
typedef enum{ taskHandleID_RPCSerialIn, taskHandleID_LED,taskHandleID_key, taskHandleID_adc, taskHandleID_count} t_taskHandleID;


void HALTAndBlink(void);


typedef enum{rer_none,
	rer_resetPin,
	rer_powerOnReset,
	rer_independendWatchdog,
	rer_windowWatchdog,
	rer_softwareReset,
	rer_rtc,
	rer_wupin1_USB,
	rer_wupin2_ONOFFKEY,
	} resetReason_t;

extern resetReason_t mainResetReason;

extern uint32_t sysTick_ms;

#endif /* MAIN_H_ */
