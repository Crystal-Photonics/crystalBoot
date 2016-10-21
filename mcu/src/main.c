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

//#include <stdio.h>
#include <assert.h>
#include "vc.h"
#include "board.h"
#include "main.h"
#include "port_flash.h"
#include "port_serial.h"

#include "rpc_receiver.h"
#include "rpc_transmission/server/generated_general/RPC_TRANSMISSION_network.h"
#include "rpc_transmission/client/generated_app/RPC_TRANSMISSION_mcu2qt.h"

#include "programmer.h"
#include "channel_codec/channel_codec.h"

#include "channel_codec/phylayer.h"
#include "errorlogger/generic_eeprom_errorlogger.h"

uint32_t sysTick_ms;



resetReason_t mainResetReason;
uint32_t FlashDestination;




#if 0
/**
 * @brief  Convert an Integer to a string
 * @param  str: The string
 * @param  intnum: The intger to be converted
 * @retval None
 */
static void Int2Str(uint8_t* str, int32_t intnum)
{
	uint32_t i, Div = 1000000000, j = 0, Status = 0;

	for (i = 0; i < 10; i++)
	{
		str[j++] = (intnum / Div) + 48;

		intnum = intnum % Div;
		Div /= 10;
		if ((str[j-1] == '0') & (Status == 0))
		{
			j = 0;
		}
		else
		{
			Status++;
		}
	}
}
#endif


static void printResetReason_t(resetReason_t reason){
	switch(reason){
	case rer_resetPin:
		printf("Reset reason: NRST Pin or Debugger\n");
		break;
	case rer_powerOnReset:
		printf("Reset reason: Power On Reset\n");
		break;
	case rer_softwareReset:
		printf("Reset reason: software reset\n");
		break;
	case rer_independendWatchdog:
		printf("Reset reason: independend Watchdog reset\n");
		break;
	case rer_windowWatchdog:
		printf("Reset reason: windowed Watchdog reset\n");
		break;
	case rer_rtc:
		printf("Reset reason: rtc wakeup reset\n");
		break;
	case rer_wupin1_USB:
		printf("Reset reason: rer_wupin1_USB reset\n");
		break;
	case rer_wupin2_ONOFFKEY:
		printf("Reset reason: rer_wupin2_ONOFFKEY reset\n");
		break;
	case rer_none:
		printf("Reset reason: unknown reset\n");
		break;

	}
}





static resetReason_t mainTestResetSource(void){
	// test the reset flags in order because the pin reset is always set.
	//http://www.micromouseonline.com/2012/03/29/stm32-reset-source/
	resetReason_t result = rer_none;
#if 0
	uint32_t rcc_csr = RCC->CSR;
	uint32_t pwr_csr = PWR->CSR;
	printf("reset reason PWR_CSR 0x%08"PRIX32 " RCC_CSR 0x%08"PRIX32 " RTC_ISR 0x%08"PRIX32"\n",rcc_csr,pwr_csr,RTC->ISR);

	if ((pwr_csr & PWR_CSR_WUF) && (pwr_csr & PWR_CSR_SBF)){
		//PWR_CSR_WUF: Wakeup flag
		//	This bit is set by hardware and cleared by a system reset or by setting the CWUF bit in the
		//	PWR power control register (PWR_CR)
		//	0: No wakeup event occurred
		//	1: A wakeup event was received from the WKUP pin or from the RTC alarm (Alarm A or
		//	Alarm B), RTC Tamper event, RTC TimeStamp event or RTC Wakeup).

		//PWR_CSR_SBF: Standby flag
		//	This bit is set by hardware and cleared only by a POR/PDR (power on reset/power down reset)
		//	or by setting the CSBF bit in the PWR power control register (PWR_CR)
		//	0: Device has not been in Standby mode
		//	1: Device has been in Standby mode

		//probe was in standbymode
		  if (GET_KEYONOFF()){
			  result = rer_wupin2_ONOFFKEY; //works

			  while(GET_KEYONOFF()){ //lets wait until key released

			  }

		  }else if(RTC->ISR & RTC_ISR_WUTF){
			  result = rer_rtc; 			//works
			  HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);
		  }else{

			  result = rer_none;
			  //assert(0);
		  }

	}else if  ( rcc_csr & RCC_CSR_LPWRRSTF){

		//Low-power management reset
		//	There are two ways to generate a low-power management reset:
		//
		//	1.Reset generated when entering Standby mode:
		//		This type of reset is enabled by resetting nRST_STDBY bit in user option bytes. In this
		//		case, whenever a Standby mode entry sequence is successfully executed, the device
		//		is reset instead of entering Standby mode.
		//
		//	2. Reset when entering Stop mode:
		//		This type of reset is enabled by resetting nRST_STOP bit in user option bytes. In this
		//		case, whenever a Stop mode entry sequence

		//shouldnt happen..
		result = rer_none;
		assert(0);


	}else if  ( rcc_csr & RCC_CSR_IWDGRSTF){
		result = rer_independendWatchdog;
	}else if  ( rcc_csr & RCC_CSR_WWDGRSTF){
		result = rer_windowWatchdog;
	}else if  ( rcc_csr & RCC_CSR_SFTRSTF){
		result = rer_softwareReset;
	} else if  ( rcc_csr & RCC_CSR_PORRSTF){
		 result = rer_powerOnReset;  //works
	} else if  (rcc_csr & RCC_CSR_PINRSTF){
		 result = rer_resetPin;   //works
	} else {
		result = rer_none;
		assert(0);
	}

	RCC->CSR |= RCC_CSR_RMVF; 				//Remove reset flag
	PWR->CR |= PWR_CR_CSBF | PWR_CR_CWUF;  	// Clear Wakeup Flag  Clear Standby Flag
	HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);	//System reset, as well as low-power modes (Sleep, Stop and Standby) have no influence on the wakeup timer.


#endif
	return result;
}


bool testIfStartIntoProgrammingMode(){
	return true;
}


int main(void)
{

	bool hardreset=true;
	 /* Flash unlock */
	//FLASH_Unlock();

	/* Initialize Key Button mounted on STM3210X-EVAL board */

//	portFlashRunApplication();

	boardConfigurePIO();
	portSerialInit(115200);




	mainResetReason = mainTestResetSource();
	printResetReason_t(mainResetReason);
#if 1
	printf("reset reason %" PRIu32 "NRST:%d \n", RCC->CSR,hardreset);
	printf("githash = %X\n", GITHASH);
	printf("gitdate = %s %u\n", GITDATE, GITUNIX);
#endif
	SET_LED_RED();
	if(mainResetReason == rer_rtc){


	}else if(mainResetReason == rer_resetPin){


	}



	rpc_receiver_init();

	/* Test if Key push-button on STM3210X-EVAL Board is pressed */
	if (testIfStartIntoProgrammingMode())
	{


		while (1)
		{
			static uint32_t oldTick;
			uint32_t tick = sysTick_ms/100;
			rpc_receive();
			//printf("Halloasdasdasdasdasdasdasdasdasdasdasdasdasdasdasd\n");
			if (oldTick != tick){
				if (tick & 1){
					SET_LED_RED();
					//programmerErase();
				}else{
					CLEAR_LED_RED();
					//printf("hallo\n");
					//portSerialPutString("Hallo\n");
				}
			}
			oldTick = tick;
			//sysTick_ms++;
		}

	}
	/* Keep the user application running */
	else
	{
#if 0
		/* Test if user code is programmed starting from address "ApplicationAddress" */
		if (((*(__IO uint32_t*)ApplicationAddress) & 0x2FFE0000 ) == 0x20000000)
		{
			/* Jump to user application */
			JumpAddress = *(__IO uint32_t*) (ApplicationAddress + 4);
			Jump_To_Application = (pFunction) JumpAddress;
			/* Initialize user application's Stack Pointer */
			__set_MSP(*(__IO uint32_t*) ApplicationAddress);
			Jump_To_Application();
		}
#endif
	}

	while(1){

	}




}

#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

