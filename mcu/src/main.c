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
#include "bootloader_config.h"
#include "port_chip.h"
#include "port_board.h"
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

typedef enum{blm_none, blm_direct_into_bootloader_mode,blm_direct_to_application, blm_timeout_waiting_till_communication} bootloaderJumpMode_t;


#if BOOTLOADER_ALLOW_PAIN_TEXT_COMMUNICATION == 0 && BOOTLOADER_WITH_DECRYPT_SUPPORT == 0
#error "this way no communication is possible"
#endif

#ifndef BOORLOADER_WAITTIME_FOR_APP_BOOT_ms
#error "please define this"
#endif

uint32_t sysTick_ms;

static volatile bootloaderJumpMode_t blJumpMode = blm_none;


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

#if 0
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

	case rer_wupin:
		printf("Reset reason: rer_wupin2_ONOFFKEY reset\n");
		break;
	case rer_none:
		printf("Reset reason: unknown reset\n");
		break;

	}
}
#endif







bool testIfStartIntoProgrammingMode(){
	return true;
}



void mainEnterProgrammingMode(){
	blJumpMode = blm_direct_into_bootloader_mode;
}

int main(void)
{

	bool hardreset=true;
	blJumpMode = blm_timeout_waiting_till_communication;

	boardInit();
	if (getEnterBootloaderKeyState()){
		blJumpMode = blm_direct_into_bootloader_mode;
	}

	resetReason_t resetReason = portTestResetSource();

	switch (resetReason){
		case rer_none:
		case rer_resetPin:
		case rer_powerOnReset:
			break;
		case rer_softwareReset:
			if (port_isDirectApplicationLaunchProgrammed()){
				blJumpMode = blm_direct_to_application;
			}
			break;



		case rer_independendWatchdog:
		case rer_windowWatchdog:
		case rer_rtc:
		case rer_wupin:
			blJumpMode = blm_direct_to_application;
			break;

	}
	if(blJumpMode == blm_direct_to_application){
		portFlashRunApplication();
		while(1){

		}
	}
	port_chipInit();
	portSerialInit(115200);

	//printResetReason_t(mainResetReason);
#if 1
	printf("reset reason %" PRIu32 "NRST:%d \n", RCC->CSR,hardreset);
	printf("githash = %X\n", GITHASH);
	printf("gitdate = %s %u\n", GITDATE, GITUNIX);
#endif
	SET_LED_BUSY();

	rpc_receiver_init();
	uint32_t startSysTick = sysTick_ms;
	while (1)
	{
		static uint32_t oldTick100ms;

		uint32_t tick100ms = sysTick_ms/100;
		rpc_receive();

		if ((blJumpMode == blm_timeout_waiting_till_communication) && ((sysTick_ms - startSysTick) > BOORLOADER_WAITTIME_FOR_APP_BOOT_ms)){
			programmerRunApplication();

		}
		if (oldTick100ms != tick100ms){
			if (tick100ms & 1){
				SET_LED_BUSY();
			}else{
				CLEAR_LED_BUSY();
				//printf("hallo\n");
				//portSerialPutString("Hallo\n");
			}
		}
		oldTick100ms = tick100ms;
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

