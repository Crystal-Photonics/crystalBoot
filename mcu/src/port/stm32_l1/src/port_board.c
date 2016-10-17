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

	//__HAL_RCC_COMP_CLK_ENABLE();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
#if 0
	  /* MemoryManagement_IRQn interrupt configuration */
	  HAL_NVIC_SetPriority(MemoryManagement_IRQn, 0, 0);
	  /* BusFault_IRQn interrupt configuration */
	  HAL_NVIC_SetPriority(BusFault_IRQn, 0, 0);
	  /* UsageFault_IRQn interrupt configuration */
	  HAL_NVIC_SetPriority(UsageFault_IRQn, 0, 0);
	  /* DebugMonitor_IRQn interrupt configuration */
	  HAL_NVIC_SetPriority(DebugMonitor_IRQn, 0, 0);
#endif
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




