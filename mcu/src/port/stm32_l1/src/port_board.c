/*
 *      Author: ak
 * Copyright (C) 2015  Crystal-Photonics GmbH
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

#include "board.h"



const pinGPIO_t gpioPins[] = { PIN_LED_RED,PIN_KEY_1,PIN_DBG_TX_PIO};


const pinGPIO_t afPins[] = {PIN_DBG_RX		,PIN_DBG_TX
};

#if 0
#define portDISABLE_INTERRUPTS()				ulPortSetInterruptMask()
#define portENABLE_INTERRUPTS()					vPortClearInterruptMask(0)


__attribute__(( naked )) uint32_t ulPortSetInterruptMask( void )
{
	__asm volatile														\
	(																	\
		"	mrs r0, basepri											\n" \
		"	mov r1, %0												\n"	\
		"	msr basepri, r1											\n" \
		"	bx lr													\n" \
		:: "i" ( configMAX_SYSCALL_INTERRUPT_PRIORITY ) : "r0", "r1"	\
	);

	/* This return will not be reached but is necessary to prevent compiler
	warnings. */
	return 0;
}
/*-----------------------------------------------------------*/

__attribute__(( naked )) void vPortClearInterruptMask( uint32_t ulNewMaskValue )
{
	__asm volatile													\
	(																\
		"	msr basepri, r0										\n"	\
		"	bx lr												\n" \
		:::"r0"														\
	);

	/* Just to avoid compiler warnings. */
	( void ) ulNewMaskValue;
}
#endif

void vPortEnterCritical( void )
{
#if 0
	portDISABLE_INTERRUPTS();

	__asm volatile( "dsb" );
	__asm volatile( "isb" );
#else
	  __disable_irq();
#endif
}

void vPortExitCritical( void )
{
	#if 0
	portENABLE_INTERRUPTS();
#else
	__enable_irq();
#endif
}

/**
  * @brief  Configure a SysTick Base time to 10 ms.
  * @param  None
  * @retval None
  */
static void SysTickConfig(void)
{
	/* Setup SysTick Timer for 1ms interrupts  */
	if (SysTick_Config(SystemCoreClock / 1000))
	{
		/* Capture error */
		while (1);
	}
	/* Configure the SysTick handler priority */
	NVIC_SetPriority(SysTick_IRQn, 0x0);
}

void boardPowerOn(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
}




void boardConfigurePIO(void){
	size_t i;

	SystemCoreClockUpdate();

	SysTickConfig();

	boardPowerOn();

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);

	for (i=0;i<sizeof(gpioPins)/sizeof(pinGPIO_t);i++){
		GPIO_Init(gpioPins[i].port, (GPIO_InitTypeDef*) &gpioPins[i].pinDef);
	}

	for (i=0;i<sizeof(afPins)/sizeof(pinGPIO_t);i++){
		GPIO_Init(afPins[i].port, (GPIO_InitTypeDef*) &afPins[i].pinDef);
	}

	for (i=0;i<sizeof(afPins)/sizeof(pinGPIO_t);i++){
		GPIO_PinAFConfig(afPins[i].port, afPins[i].pinSource, afPins[i].af);
	}
}




