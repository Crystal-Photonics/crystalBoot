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



#define RTC_CLOCK_SOURCE_LSE 1

resetReason_t mainResetReason;
TaskHandle_t	taskHandles[taskHandleID_count];

void mainSwitchIntoLowPower(void){



	//lowPowerStartLP(STM32L_STANDBY, RTC_STATE_ON);
	//after a Standby-wakeup the system will be reset
	while(1){}
}


/**
  * @brief  Configure the RTC peripheral by selecting the clock source.
  * @param  None
  * @retval None
  */
void RTC_Config(void)
{
	  RTC_InitTypeDef RTC_InitStructure;

	/* Enable the PWR clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

	/* Allow access to RTC */
	PWR_RTCAccessCmd(ENABLE);

#if defined (RTC_CLOCK_SOURCE_LSI)  /* LSI used as RTC source clock*/
	/* The RTC Clock may varies due to LSI frequency dispersion. */
	/* Enable the LSI OSC */
	RCC_LSICmd(ENABLE);

	/* Wait till LSI is ready */
	while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
	{
	}

	/* Select the RTC Clock Source */
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);

	SynchPrediv = 0xFF;
	AsynchPrediv = 0x7F;

#elif defined (RTC_CLOCK_SOURCE_LSE) /* LSE used as RTC source clock */
	/* Enable the LSE OSC */
	RCC_LSEConfig(RCC_LSE_ON);

	/* Wait till LSE is ready */
	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
	{
	}

	/* Select the RTC Clock Source */
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

	  /* Calendar Configuration */
	#if 1
	  RTC_InitStructure.RTC_AsynchPrediv = 127;
	  RTC_InitStructure.RTC_SynchPrediv =  255; //=1Hz
	  RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
	  RTC_Init(&RTC_InitStructure);
	#endif

#else
#error Please select the RTC Clock source inside the main.c file
#endif /* RTC_CLOCK_SOURCE_LSI */

	/* Enable the RTC Clock */
	RCC_RTCCLKCmd(ENABLE);

	/* Wait for RTC APB registers synchronisation */
	RTC_WaitForSynchro();
	PWR_RTCAccessCmd(ENABLE);
}



void RTC_SetToDefaulDate(void)
{
	RTC_TimeTypeDef RTC_TimeStructure;
	RTC_DateTypeDef RTC_DateStructure;
	/* Allow access to RTC */
	PWR_RTCAccessCmd(ENABLE);

	RTC_TimeStructure.RTC_H12     = RTC_H12_AM;
	RTC_TimeStructure.RTC_Hours = 0;
	RTC_TimeStructure.RTC_Minutes = 0;
	RTC_TimeStructure.RTC_Seconds = 0;
	RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure);

	RTC_DateStructure.RTC_WeekDay = 3;
	RTC_DateStructure.RTC_Month = 01;
	RTC_DateStructure.RTC_Date = 01;
	RTC_DateStructure.RTC_Year = 0x15;
	RTC_SetDate(RTC_Format_BCD, &RTC_DateStructure);
	PWR_RTCAccessCmd(DISABLE);
}




void mainBackup_SetLEDStatus(uint32_t ledStatus){
	PWR_RTCAccessCmd(ENABLE);
	RTC_WriteBackupRegister(RTC_BKP_DR0,ledStatus);
	PWR_RTCAccessCmd(DISABLE);
}

uint32_t mainBackup_GetLEDStatus(void){
	uint32_t result;
	result = RTC_ReadBackupRegister(RTC_BKP_DR0);
	return result;
}



void initRTCInterrupt(){
	  NVIC_InitTypeDef NVIC_InitStructure;
	  EXTI_InitTypeDef EXTI_InitStructure;

	  PWR_RTCAccessCmd(ENABLE);

	  EXTI_ClearITPendingBit(EXTI_Line20);
	  EXTI_InitStructure.EXTI_Line = EXTI_Line20;
	  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	  EXTI_Init(&EXTI_InitStructure);

	  /* Enable the RTC Wakeup Interrupt */
	  NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn;
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =  configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY+1;
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	  NVIC_Init(&NVIC_InitStructure);

	 // RTC_WakeUpCmd(DISABLE);
	  /* 3.1.3 from AN3371 using hardware RTC
	  WUCKSEL -> (only when RTC->CR WUTE = 0; RTC->ISR WUTWF = 1)
	  000: RTC/16 clock is selected
	  001: RTC/8 clock is selected
	  010: RTC/4 clock is selected
	  011: RTC/2 clock is selected
	  10x: ck_spre (usually 1 Hz) clock is selected
	  11x: ck_spre (usually 1 Hz) clock is selected and 216 is added to the WUT counter value
	  from 1s to 18 hours when WUCKSEL [2:1] = 10
	  */
	  /* Clear Wake-up flag */
	  //PWR->CR |= PWR_CR_CWUF;

	  //RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);
	  //RTC_SetWakeUpCounter(period_s-1);
	  //The WUTF flag must then be cleared by software.
	  RTC_ClearITPendingBit(RTC_IT_WUT); //ClearITPendingBit clears also the flag
	  RTC_ClearFlag(RTC_FLAG_WUTF); //MANDATORY!
	  RTC_ITConfig(RTC_IT_WUT, ENABLE); //enable interrupt
	  //RTC_WakeUpCmd(ENABLE);
	  PWR_RTCAccessCmd(DISABLE); //just in case
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



	RPC_TRANSMISSION_RESULT result = qtKeyPressed(keyStatus);

	if (result == RPC_TRANSMISSION_SUCCESS){
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
	/* Configure the hardware ready to run the test. */
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );
	boardConfigurePIO();
#if 0
	for (;;){
		static uint8_t ledstatus = 0;
		if (ledstatus & 1){
			SET_LED_BLUE();
			SET_LED_RED();
		}else{
			CLEAR_LED_BLUE();
			CLEAR_LED_RED();
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
			RTC_WakeUpCmd(DISABLE);
		}

	}
#endif
	if(hardreset){
		mainResetReason = rer_reset;
		RTC_Config();
		RTC_SetToDefaulDate();
		for (uint32_t i = 0; i < RTC_BKP_DR19;i++){
			RTC_WriteBackupRegister(i,0);
		}

	}


	PWR->CR |= PWR_CR_CSBF;//reset standby flag
	PWR->CR |= PWR_CR_CWUF;

	PWR_RTCAccessCmd(ENABLE);
	RCC_LSEConfig( ENABLE );
	PWR_RTCAccessCmd(DISABLE);
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
			GPIO_ToggleBits(GPIOA, GPIO_Pin_8);
			GPIO_ToggleBits(GPIOB, GPIO_Pin_7);
		}
	}
}
#endif

///*-----------------------------------------------------------*/
void assert_failed(uint8_t* file, uint32_t line){

}
