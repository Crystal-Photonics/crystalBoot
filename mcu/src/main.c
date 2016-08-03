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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"

#include "board.h"
#include "serial.h"

#include "task_rpc_serial_in.h"
#include "task_led.h"
#include "task_key.h"
#include "task_adc.h"

#include "rpc_transmission/server/generated_general/RPC_TRANSMISSION_network.h"
#include "rpc_transmission/client/generated_app/RPC_TRANSMISSION_mcu2qt.h"


#include "lowpower.h"
#include "vc.h"

channel_codec_instance_t cc_instances[channel_codec_comport_COUNT];

#define RTC_CLOCK_SOURCE_LSE 1

resetReason_t mainResetReason;
TaskHandle_t	taskHandles[taskHandleID_count];
SemaphoreHandle_t semaphoreWakeUp;

void mainSwitchIntoLowPower(void){



	//lowPowerStartLP(STM32L_STANDBY, RTC_STATE_ON);
	//after a Standby-wakeup the system will be reset
	while(1){}
}


void RTC_SetToDefaulDate(void)
{
	  RTC_TimeTypeDef sTime;
	  RTC_DateTypeDef sDate;

	  sTime.Hours = 0x0;
	  sTime.Minutes = 0x0;
	  sTime.Seconds = 0x0;
	  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
	  HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD);

	  sDate.WeekDay = RTC_WEEKDAY_MONDAY;
	  sDate.Month = RTC_MONTH_JANUARY;
	  sDate.Date = 0x1;
	  sDate.Year = 0x0;

	  HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD);
}




void mainBackup_SetLEDStatus(uint32_t ledStatus){
	HAL_RTCEx_BKUPWrite(&hrtc,RTC_BKP_DR0,ledStatus);
}

uint32_t mainBackup_GetLEDStatus(void){
	uint32_t result;
	result = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR0);
	return result;
}



void initRTCInterrupt(){
	HAL_NVIC_SetPriority(RTC_WKUP_IRQn,configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY+1,0);
	HAL_RTCEx_SetWakeUpTimer_IT(&hrtc,1000,RTC_WAKEUPCLOCK_CK_SPRE_16BITS);
}

void keyPressHandle (key_event_t event, key_id_t id){
	(void)id;

	if (id == kid_key1){
		if (event == ke_release){
			ledPatternSetToOff(lid_blue, lpp_priority_1);
			//ledPatternSetToBlink(lid_red, lpp_priority_1, 1, 10, 10, 3);
		}else if (event == ke_pressShort){
			ledPatternSetToBlink(lid_blue, lpp_priority_1, 1, 2, 10, 3);
			//ledPatternSetToOff(lid_red, lpp_priority_1);
		}
	}
	rpcKeyStatus_t keyStatus=rpcKeyStatus_none;
	switch(event){
	case ke_none:
		keyStatus = rpcKeyStatus_none;
		break;
	case ke_pressShort:
		keyStatus = rpcKeyStatus_pressed;
		break;
	case ke_pressLong:
		keyStatus = rpcKeyStatus_pressedLong;
		break;
	case ke_release:
		keyStatus = rpcKeyStatus_released;
		break;
	}



	RPC_RESULT result = qtKeyPressed(keyStatus);

	if (result == RPC_SUCCESS){
		ledPatternSetToBlink(lid_red, lpp_priority_1, 1, 10, 10, 3);
	}else {
		ledPatternSetToOff(lid_red, lpp_priority_1);
	}

}

int main(void)
{
	int n;
	bool hardreset=false;
	uint32_t ledstatus;
	HAL_Init();

	SystemClock_Config();

	boardConfigurePIO();

#if 0
	for (;;){
		static uint8_t ledstatus = 0;
		if (ledstatus & 1){
			CLEAR_LED_BLUE();
			CLEAR_LED_RED();
		}else{
			SET_LED_BLUE();
			SET_LED_RED();
		}
		ledstatus++;
		for (int i=0; i<250000; i++);
	}
#endif

	SystemCoreClockUpdate();
	xSerialPortInitMinimal(115200,50);

	semaphoreWakeUp = xSemaphoreCreateBinary();
	RPC_TRANSMISSION_mutex_init();

    printf("reset reason %" PRIu32 "NRST:%d \n", RCC->CSR,hardreset);
    printf("githash = %X\n", GITHASH);
    printf("gitdate = %s %u\n", GITDATE, GITUNIX);
    RCC->CSR |=RCC_CSR_RMVF;
#if 1
    if (PWR->CSR & PWR_CSR_WUF){
		for (int i=0; i<2500; i++);

		mainResetReason = rer_rtc;
		if (GET_KEYONOFF()){
			mainResetReason = rer_wupin2_ONOFFKEY;
			SET_LED_GREEN();
			while(GET_KEYONOFF()){}	//wait until key released
			CLEAR_LED_GREEN();

			initRTCInterrupt();
		}

		if(mainResetReason == rer_rtc){
			HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);

		}

	}
#endif
	if(hardreset){
		mainResetReason = rer_reset;
		MX_RTC_Init();
		RTC_SetToDefaulDate();
		for (uint32_t i = 0; i < RTC_BKP_DR19;i++){
			HAL_RTCEx_BKUPWrite(&hrtc,i,0);
		}

	}


	PWR->CR |= PWR_CR_CSBF;//reset standby flag
	PWR->CR |= PWR_CR_CWUF;


	keyInit();

	keyRegisterHandle(&keyPressHandle);

#if 1

	/* If all is well, the scheduler will now be running, and the following line
	will never be reached.  If the following line does execute, then there was
	insufficient FreeRTOS heap memory available for the idle and/or timer tasks
	to be created.  See the memory management section on the FreeRTOS web site
	for more details. */



	xTaskCreate( taskLED, "LED", mainLED_TASK_STACK/*stack*/, NULL, mainLED_TASK_PRIORITY /*prior*/, &taskHandles[taskHandleID_LED] );
	xTaskCreate( taskKey, "KEY", mainKEY_TASK_STACK/*stack*/, NULL, mainKEY_TASK_PRIORITY /*prior*/, &taskHandles[taskHandleID_key] );
	xTaskCreate( taskRPCSerialIn, "RPC", mainRPC_SERIAL_TASK_STACK/*stack*/, NULL, mainRPC_TASK_SERIAL_PRIORITY /*prior*/, &taskHandles[taskHandleID_RPCSerialIn] );
	xTaskCreate( taskADC, "ADC", mainADC_TASK_STACK/*stack*/, NULL, mainADC_TASK_PRIORITY /*prior*/, &taskHandles[taskHandleID_adc] );

    for (int i = 0;i < taskHandleID_count; i++){
    	if ((i != taskHandleID_RPCSerialIn))
    		vTaskSuspend(taskHandles[i]);
    }

	serialSetRTOSRunningFlag(true);
	vTaskStartScheduler();
	for (;;) {}
#endif

}



/*-----------------------------------------------------------*/

void vApplicationTickHook( void )
{
#if ( mainCREATE_SIMPLE_LED_FLASHER_DEMO_ONLY == 0 )
	{

		/* Fill the FPU registers with 0. */
		//vRegTestClearFlopRegistersToParameterValue( 0UL );

		/* Trigger a timer 2 interrupt, which will fill the registers with a
		different value and itself trigger a timer 3 interrupt.  Note that the
		timers are not actually used.  The timer 2 and 3 interrupt vectors are
		just used for convenience. */
		NVIC_SetPendingIRQ( TIM2_IRQn );

		/* Ensure that, after returning from the nested interrupts, all the FPU
		registers contain the value to which they were set by the tick hook
		function. */
		//configASSERT( ulRegTestCheckFlopRegistersContainParameterValue( 0UL ) );


	}
#endif
}

/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
#if 0
	/* vApplicationMallocFailedHook() will only be called if
	configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
	function that will get called if a call to pvPortMalloc() fails.
	pvPortMalloc() is called internally by the kernel whenever a task, queue,
	timer or semaphore is created.  It is also called by various parts of the
	demo application.  If heap_1.c or heap_2.c are used, then the size of the
	heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
	FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
	to query the size of free heap space that remains (although it does not
	provide information on how the remaining heap might be fragmented). */
	taskDISABLE_INTERRUPTS();
#endif
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
	/* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
	to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
	task.  It is essential that code added to this hook function never attempts
	to block in any way (for example, call xQueueReceive() with a block time
	specified, or call vTaskDelay()).  If the application makes use of the
	vTaskDelete() API function (as this demo application does) then it is also
	important that vApplicationIdleHook() is permitted to return to its calling
	function, because it is the responsibility of the idle task to clean up
	memory allocated by the kernel to any task that has since been deleted. */
}
/*-----------------------------------------------------------*/
#if 1
void vApplicationStackOverflowHook( xTaskHandle pxTask, signed char *pcTaskName )
{

	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	taskDISABLE_INTERRUPTS();
	SET_LED_BLUE();
	CLEAR_LED_RED();
	for( ;; ){
		for (int n=0;n<10;n++){
			// waste time
			for (int i=0; i<250000; i++);
			{}
			HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_8);
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7);
		}
	}
}
#endif

///*-----------------------------------------------------------*/
void assert_failed(uint8_t* file, uint32_t line){

}
