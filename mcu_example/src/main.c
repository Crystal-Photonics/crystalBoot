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

#include <stdio.h>
#include <inttypes.h>
#include <assert.h>
#include "main.h"

#include "board.h"



#include "vc.h"


uint32_t sysTick_ms=0;

void iniSysTick(void)
{


//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);


  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);

}



int main(void)
{
	int n;
	bool hardreset=false;
	uint32_t ledstatus;
	HAL_Init();
	__enable_irq();			//interrupts are disabled


	boardConfigurePIO();
	SET_LED_BLUE();
	CLEAR_LED_RED();


	SystemCoreClockUpdate();

	iniSysTick();






	uint32_t startSysTick = sysTick_ms;
	while (1)
	{
		static uint32_t oldTick100ms;

		uint32_t tick100ms = sysTick_ms/100;
	//	sysTick_ms++;
		if (oldTick100ms != tick100ms){
			if (tick100ms & 1){
				SET_LED_RED();
				CLEAR_LED_BLUE();
			}else{
				CLEAR_LED_RED();
				SET_LED_BLUE();
			}
		}
		oldTick100ms = tick100ms;
	}



}



/*-----------------------------------------------------------*/

///*-----------------------------------------------------------*/
void assert_failed(uint8_t* file, uint32_t line){

}
