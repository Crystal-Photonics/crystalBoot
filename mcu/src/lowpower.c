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

#include "lowpower.h"

#include "main.h"
#include "task_led.h"




SemaphoreHandle_t semaphoreWakeUp;

void lowPowerInitPeriodicWakeUp(uint16_t period_s)
{

  NVIC_InitTypeDef NVIC_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;

  /* Enable the PWR clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

  /* Allow access to RTC */
  PWR_RTCAccessCmd(ENABLE);

  /*
  To enable the RTC Wakeup interrupt, the following sequence is required:
  1. Configure and enable the EXTI Line 20 in interrupt mode and select the rising edge
  sensitivity.
  2. Configure and enable the RTC_WKUP IRQ channel in the NVIC.
  3. Configure the RTC to generate the RTC wakeup timer event.

  System reset, as well as low power modes (Sleep, Stop and Standby) have no influence on
  the wakeup timer.
  */
  /* EXTI configuration *******************************************************/
  EXTI_ClearITPendingBit(EXTI_Line20);
  EXTI_InitStructure.EXTI_Line = EXTI_Line20;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Enable the RTC Wakeup Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY+1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  RTC_WakeUpCmd(DISABLE);
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
  PWR->CR |= PWR_CR_CWUF;

  RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);
  RTC_SetWakeUpCounter(period_s-1);
  //The WUTF flag must then be cleared by software.
  RTC_ClearITPendingBit(RTC_IT_WUT); //ClearITPendingBit clears also the flag
  RTC_ClearFlag(RTC_FLAG_WUTF); //MANDATORY!
  RTC_ITConfig(RTC_IT_WUT, ENABLE); //enable interrupt
  RTC_WakeUpCmd(ENABLE);
  PWR_RTCAccessCmd(DISABLE); //just in case
}

__IO uint8_t LowPowerStatus = 0x00;




/**
 * @brief  Configures system clock after wake-up from STOP: enable HSE, PLL
 *         and select PLL as system clock source.
 * @param  None
 * @retval None
 */
void IDD_Measurement_SYSCLKConfig_STOP(void)
{
	ErrorStatus HSEStartUpStatus;

	/* Enable HSE */
	RCC_HSEConfig(RCC_HSE_ON);

	/* Wait till HSE is ready */
	HSEStartUpStatus = RCC_WaitForHSEStartUp();

	if (HSEStartUpStatus == SUCCESS)
	{
		/* Enable PLL */
		RCC_PLLCmd(ENABLE);

		/* Wait till PLL is ready */
		while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
		{}

		/* Select PLL as system clock source */
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

		/* Wait till PLL is used as system clock source */
		while (RCC_GetSYSCLKSource() != 0x0C)
		{}
	}
}


/**
 * @brief  Selects HSI as System clock source and configure HCLK, PCLK2 and PCLK1 prescalers.
 * @param  None
 * @retval None
 */
void SetHCLKToHSI(void)
{
	__IO uint32_t StartUpCounter = 0, HSIStatus = 0;

	/* SYSCLK, HCLK, PCLK2 and PCLK1 configuration -----------------------------*/
	/* RCC system reset(for debug purpose) */
	RCC_DeInit();

	/* Enable HSI */
	RCC_HSICmd(ENABLE);

	/* Wait till HSI is ready and if Time out is reached exit */
	do
	{
		HSIStatus = RCC_GetFlagStatus(RCC_FLAG_HSIRDY);
		StartUpCounter++;
	}
	while ((HSIStatus == 0) && (StartUpCounter != HSI_STARTUP_TIMEOUT));


	if (RCC_GetFlagStatus(RCC_FLAG_HSIRDY) != RESET)
	{
		HSIStatus = (uint32_t)0x01;
	}
	else
	{
		HSIStatus = (uint32_t)0x00;
	}

	if (HSIStatus == 0x01)
	{
		/* Flash 0 wait state */
		FLASH_SetLatency(FLASH_Latency_0);

		/* Disable Prefetch Buffer */
		FLASH_PrefetchBufferCmd(DISABLE);

		/* Disable 64-bit access */
		FLASH_ReadAccess64Cmd(DISABLE);

		/* Enable the PWR APB1 Clock */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

		/* Select the Voltage Range 1 (1.8V) */
		PWR_VoltageScalingConfig(PWR_VoltageScaling_Range1);

		/* Wait Until the Voltage Regulator is ready */
		while (PWR_GetFlagStatus(PWR_FLAG_VOS) != RESET)
		{}

		/* HCLK = SYSCLK */
		RCC_HCLKConfig(RCC_SYSCLK_Div1);

		/* PCLK2 = HCLK */
		RCC_PCLK2Config(RCC_HCLK_Div1);

		/* PCLK1 = HCLK */
		RCC_PCLK1Config(RCC_HCLK_Div1);

		/* Select HSI as system clock source */
		RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);

		/* Wait till HSI is used as system clock source */
		while (RCC_GetSYSCLKSource() != 0x04)
		{}
	}
	else
	{ /* If HSI fails to start-up, the application will have wrong clock configuration.
                               User can add here some code to deal with this error */

		/* Go to infinite loop */
		while (1)
		{}
	}
}



/**
 * @brief  Selects MSI (64KHz) as System clock source and configure
 *         HCLK, PCLK2 and PCLK1 prescalers.
 * @param  None
 * @retval None
 */
void SetHCLKToMSI_64KHz(void)
{
	/* RCC system reset */
	RCC_DeInit();

	/* Flash 0 wait state */
	FLASH_SetLatency(FLASH_Latency_0);

	/* Disable Prefetch Buffer */
	FLASH_PrefetchBufferCmd(DISABLE);

	/* Disable 64-bit access */
	FLASH_ReadAccess64Cmd(DISABLE);

	/* Enable the PWR APB1 Clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

	/* Select the Voltage Range 3 (1.2V) */
	PWR_VoltageScalingConfig(PWR_VoltageScaling_Range3);

	/* Wait Until the Voltage Regulator is ready */
	while (PWR_GetFlagStatus(PWR_FLAG_VOS) != RESET)
	{}
	/* HCLK = SYSCLK */
	RCC_HCLKConfig(RCC_SYSCLK_Div1);

	/* PCLK2 = HCLK */
	RCC_PCLK2Config(RCC_HCLK_Div1);

	/* PCLK1 = HCLK */
	RCC_PCLK1Config(RCC_HCLK_Div1);

	/* Set MSI clock range to 64KHz */
	RCC_MSIRangeConfig(RCC_MSIRange_0);

	/* Select MSI as system clock source */
	RCC_SYSCLKConfig(RCC_SYSCLKSource_MSI);

	/* Wait till PLL is used as system clock source */
	while (RCC_GetSYSCLKSource() != 0x00)
	{}
}


/**
 * @brief  Selects MSI (1MHz) as System clock source and configure
 *         HCLK, PCLK2 and PCLK1 prescalers.
 * @param  None
 * @retval None
 */
void SetHCLKToMSI_1MHz(void)
{
	/* RCC system reset */
	RCC_DeInit();

	/* Flash 0 wait state */
	FLASH_SetLatency(FLASH_Latency_0);

	/* Disable Prefetch Buffer */
	FLASH_PrefetchBufferCmd(DISABLE);

	/* Disable 64-bit access */
	FLASH_ReadAccess64Cmd(DISABLE);

	/* Enable the PWR APB1 Clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

	/* Select the Voltage Range 3 (1.2V) */
	PWR_VoltageScalingConfig(PWR_VoltageScaling_Range3);

	/* Wait Until the Voltage Regulator is ready */
	while (PWR_GetFlagStatus(PWR_FLAG_VOS) != RESET)
	{}
	/* HCLK = SYSCLK */
	RCC_HCLKConfig(RCC_SYSCLK_Div1);

	/* PCLK2 = HCLK */
	RCC_PCLK2Config(RCC_HCLK_Div1);

	/* PCLK1 = HCLK */
	RCC_PCLK1Config(RCC_HCLK_Div1);

	/* Set MSI clock range to 1MHz */
	RCC_MSIRangeConfig(RCC_MSIRange_4);

	/* Select MSI as system clock source */
	RCC_SYSCLKConfig(RCC_SYSCLKSource_MSI);

	/* Wait till PLL is used as system clock source */
	while (RCC_GetSYSCLKSource() != 0x00)
	{}
}


/**
 * @brief  Enter the MCU selected low power modes.
 * @param  lpmode: selected MCU low power modes. This parameter can be one of the
 *         following values:
 *              @arg STM32L_RUN: Run mode at 32MHz.
 *              @arg STM32L_RUN_1M: Run mode at 1MHz.
 *              @arg STM32L_RUN_LP: Low power Run mode at 32KHz.
 *              @arg STM32L_SLEEP: Sleep mode at 16MHz.
 *              @arg STM32L_SLEEP_LP: Low power Sleep mode at 32KHz.
 *              @arg STM32L_STOP: Stop mode with or without RTC.
 *              @arg STM32L_STANDBY: Standby mode with or without RTC.
 * @param  RTCState: RTC peripheral state during low power modes. This parameter
 *         is valid only for STM32L_RUN_LP, STM32L_SLEEP_LP, STM32L_STOP and
 *         STM32L_STANDBY. This parameter can be one of the following values:
 *              @arg RTC_STATE_ON: RTC peripheral is ON during low power modes.
 *              @arg RTC_STATE_OFF: RTC peripheral is OFF during low power modes.
 * @param  CalibrationState: Bias Calibration mode selection state during low
 *         power modes.
 *         This parameter can be one of the following values:
 *              @arg BIAS_CALIB_OFF: Bias Calibration mode is selected during
 *                   low power modes.
 *              @arg BIAS_CALIB_ON: Bias Calibration mode isn't selected during
 *                   low power modes.
 * @retval None
 */
void lowPowerStartLP(uint32_t lpmode, uint8_t RTCState)
{
	/* STM32L Low Power Modes definition */


	static __IO uint32_t RCC_AHBENR = 0, RCC_APB2ENR = 0, RCC_APB1ENR = 0;
	GPIO_InitTypeDef GPIO_InitStructure;
	uint16_t  adcdata, i;

	/* Disable the Wakeup Interrupt */
	RTC_ITConfig(RTC_IT_WUT, DISABLE);

	/* Save the RCC configuration */
	RCC_AHBENR   = RCC->AHBENR;
	RCC_APB2ENR  = RCC->APB2ENR;
	RCC_APB1ENR  = RCC->APB1ENR;

	/* Disable PVD */
	PWR_PVDCmd(DISABLE);

	/* Clear Wake Up flag */
	PWR_ClearFlag(PWR_FLAG_WU);

	RCC->AHBENR = RCC_AHBENR_GPIOCEN | RCC_AHBENR_GPIOAEN;
	RCC->APB2ENR = 0x00;
	//disable  	RCC_APB2ENR_SYSCFGEN,RCC_APB2ENR_TIM9EN,RCC_APB2ENR_TIM10EN,
	//			RCC_APB2ENR_TIM11EN,RCC_APB2ENR_ADC1EN,RCC_APB2ENR_SDIOEN,
	//			RCC_APB2ENR_SPI1EN,RCC_APB2ENR_USART1EN
	RCC->APB1ENR = RCC_APB1ENR_PWREN;  /* PWR APB1 Clock enable */

	switch(lpmode)
	{
	/*=========================================================================*
	 *                        RUN MODE 32MHz (HSE + PLL)                       *
	 *========================================================================*/
	case STM32L_RUN:
	{
		//mode = STM32L_MODE_RUN;
		/* Needed delay to have a correct value on capacitor C82.
        Running NOP during waiting loop will decrease the current consumption. */
		for (i = 0;i < 0xFFFF; i++)
		{
			__NOP();  __NOP();    __NOP();  __NOP();
			__NOP();  __NOP();    __NOP();  __NOP();
			__NOP();  __NOP();    __NOP();  __NOP();
			__NOP();  __NOP();    __NOP();  __NOP();
			__NOP();  __NOP();    __NOP();  __NOP();
			__NOP();  __NOP();    __NOP();  __NOP();
			__NOP();  __NOP();    __NOP();  __NOP();
			__NOP();  __NOP();    __NOP();  __NOP();
			__NOP();  __NOP();    __NOP();  __NOP();
			__NOP();  __NOP();    __NOP();  __NOP();
			__NOP();  __NOP();    __NOP();  __NOP();
		}
	}
	//while(1){}
	break;

	/*=========================================================================*
	 *                        RUN MODE MSI 1MHz                                *
	 *========================================================================*/
	case STM32L_RUN_1M:
	{
		//mode = STM32L_MODE_RUN;

		/* Reconfigure the System Clock at MSI 1 MHz */
		SetHCLKToMSI_1MHz();

		/* Needed delay to have a correct value on capacitor C82.
        Running NOP during waiting loop will decrease the current consumption. */
		for (i = 0;i < 0x3FFF; i++)
		{
			__NOP();  __NOP();    __NOP();  __NOP();
			__NOP();  __NOP();    __NOP();  __NOP();
			__NOP();  __NOP();    __NOP();  __NOP();
			__NOP();  __NOP();    __NOP();  __NOP();
			__NOP();  __NOP();    __NOP();  __NOP();
			__NOP();  __NOP();    __NOP();  __NOP();
			__NOP();  __NOP();    __NOP();  __NOP();
			__NOP();  __NOP();    __NOP();  __NOP();
			__NOP();  __NOP();    __NOP();  __NOP();
			__NOP();  __NOP();    __NOP();  __NOP();
			__NOP();  __NOP();    __NOP();  __NOP();
		}
	}
	break;

	/*=========================================================================*
	 *                        RUN LOW POWER MODE MSI 32KHz                     *
	 *========================================================================*/
	case STM32L_RUN_LP:
	{
		if(!RTCState)
		{
			RCC_LSEConfig(RCC_LSE_OFF);
		}
		else
		{
			if (RTC_GetFlagStatus(RTC_FLAG_INITS) == RESET)
			{
				/* RTC Configuration ************************************************/
				/* Reset RTC Domain */
				RCC_RTCResetCmd(ENABLE);
				RCC_RTCResetCmd(DISABLE);

				/* Enable the LSE OSC */
				RCC_LSEConfig(RCC_LSE_ON);

				/* Wait till LSE is ready */
				while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
				{}

				/* Select the RTC Clock Source */
				RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

				/* Enable the RTC Clock */
				RCC_RTCCLKCmd(ENABLE);

				/* Wait for RTC APB registers synchronisation */
				RTC_WaitForSynchro();
			}
		}
		/* Configure the System Clock at MSI 32 KHz */
		SetHCLKToMSI_64KHz();
		RCC_HCLKConfig(RCC_SYSCLK_Div2);

		/* Clear IDD_CNT_EN pin */

		/* Enter low power run mode */
		PWR_EnterLowPowerRunMode(ENABLE);

		/* Waiting wake-up interrupt */
		/* Needed delay to have a correct value on capacitor C82.
        Running NOP during waiting loop will decrease the current consumption. */
		do
		{
			__NOP();  __NOP();    __NOP();  __NOP();
			__NOP();  __NOP();    __NOP();  __NOP();
			__NOP();  __NOP();    __NOP();  __NOP();
			__NOP();  __NOP();    __NOP();  __NOP();
			__NOP();  __NOP();    __NOP();  __NOP();
			__NOP();  __NOP();    __NOP();  __NOP();
			__NOP();  __NOP();    __NOP();  __NOP();
			__NOP();  __NOP();    __NOP();  __NOP();
			__NOP();  __NOP();    __NOP();  __NOP();
			__NOP();  __NOP();    __NOP();  __NOP();
			__NOP();  __NOP();    __NOP();  __NOP();
		}while(LowPowerStatus == 0x00);

		/* Exit low power run mode before setting the clock to 32MHz */
		PWR_EnterLowPowerRunMode(DISABLE);
		while(PWR_GetFlagStatus(PWR_FLAG_REGLP) != RESET)
		{
		}
	}
	break;

	/*=========================================================================*
	 *                        SLEEP MODE HSI 16MHz                             *
	 *========================================================================*/
	case STM32L_SLEEP:
	{
		//mode = STM32L_MODE_RUN;
		/* Enable Ultra low power mode */
		PWR_UltraLowPowerCmd(ENABLE);

		/* Diable FLASH during SLeep LP */
		FLASH_SLEEPPowerDownCmd(ENABLE);

		//RCC_APB2PeriphClockLPModeCmd(RCC_APB2Periph_CLOCK, ENABLE);
		//RCC_APB1PeriphClockLPModeCmd(RCC_APB1Periph_CLOCK, ENABLE);
		//RCC_AHBPeriphClockLPModeCmd(RCC_AHBPeriph_CLOCK, ENABLE);

		/* Configure the System Clock to 16MHz */
		SetHCLKToHSI();

		/* Request to enter SLEEP mode with regulator on */
		PWR_EnterSleepMode(PWR_Regulator_ON, PWR_STOPEntry_WFI);
	}
	break;

	/*=========================================================================*
	 *                      SLEEP LOW POWER MODE MSI 32KHz                     *
	 *========================================================================*/
	case STM32L_SLEEP_LP:
	{
		if(!RTCState)
		{
			RCC_LSEConfig(RCC_LSE_OFF);
		}
		else
		{
			if (RTC_GetFlagStatus(RTC_FLAG_INITS) == RESET)
			{
				/* RTC Configuration ************************************************/
				/* Reset RTC Domain */
				RCC_RTCResetCmd(ENABLE);
				RCC_RTCResetCmd(DISABLE);

				/* Enable the LSE OSC */
				RCC_LSEConfig(RCC_LSE_ON);

				/* Wait till LSE is ready */
				while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
				{}

				/* Select the RTC Clock Source */
				RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

				/* Enable the RTC Clock */
				RCC_RTCCLKCmd(ENABLE);

				/* Wait for RTC APB registers synchronisation */
				RTC_WaitForSynchro();
			}
		}

		/* Enable Ultra low power mode */
		PWR_UltraLowPowerCmd(ENABLE);

		/* Diable FLASH during SLeep LP */
		FLASH_SLEEPPowerDownCmd(ENABLE);

		/* Disable HSI clock before entering Sleep LP mode */
		RCC_HSICmd(DISABLE);
		/* Disable HSE clock */
		RCC_HSEConfig(RCC_HSE_OFF);
		/* Disable LSI clock */
		RCC_LSICmd(DISABLE);

		//RCC_APB2PeriphClockLPModeCmd(RCC_APB2Periph_CLOCK, ENABLE);
		//RCC_APB1PeriphClockLPModeCmd(RCC_APB1Periph_CLOCK, ENABLE);
		//RCC_AHBPeriphClockLPModeCmd(RCC_AHBPeriph_CLOCK, ENABLE);

		/* Clear IDD_CNT_EN pin */
		//GPIO_ResetBits(IDD_CNT_EN_GPIO_PORT, IDD_CNT_EN_PIN);

		/* Reconfigure the System Clock at MSI 64 KHz */
		SetHCLKToMSI_64KHz();
		RCC_HCLKConfig(RCC_SYSCLK_Div2);

		/* Request to enter SLEEP mode with regulator low power */
		PWR_EnterSleepMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
	}
	break;

	/*=========================================================================*
	 *                          STOP LOW POWER MODE                            *
	 *========================================================================*/
	case STM32L_STOP:
	{
		/* Enable Ultra low power mode */
		PWR_UltraLowPowerCmd(ENABLE);

		if(!RTCState)
		{
			RCC_LSEConfig(RCC_LSE_OFF);
		}
		else
		{
			if (RTC_GetFlagStatus(RTC_FLAG_INITS) == RESET)
			{
				/* RTC Configuration ************************************************/
				/* Reset RTC Domain */
				RCC_RTCResetCmd(ENABLE);
				RCC_RTCResetCmd(DISABLE);

				/* Enable the LSE OSC */
				RCC_LSEConfig(RCC_LSE_ON);

				/* Wait till LSE is ready */
				while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
				{}

				/* Select the RTC Clock Source */
				RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

				/* Enable the RTC Clock */
				RCC_RTCCLKCmd(ENABLE);

				/* Wait for RTC APB registers synchronisation */
				RTC_WaitForSynchro();
			}
		}

		/* Request to enter STOP mode with regulator in low power */
		PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);

		/* Configures system clock after wake-up from STOP: enable HSE, PLL and select PLL
         as system clock source (HSE and PLL are disabled in STOP mode) */
		IDD_Measurement_SYSCLKConfig_STOP();
	}
	break;

	/*=========================================================================*
	 *                          STANDBY LOW POWER MODE                         *
	 *========================================================================*/
	case STM32L_STANDBY:
	{
		//PWR_WakeUpPinCmd(PWR_WakeUpPin_2, DISABLE);
		//PWR_WakeUpPinCmd(PWR_WakeUpPin_1, DISABLE);

		PWR_WakeUpPinCmd(PWR_WakeUpPin_2, ENABLE);
		PWR_WakeUpPinCmd(PWR_WakeUpPin_1, ENABLE);
		PWR_UltraLowPowerCmd(ENABLE);
		boardSetPinsToIntput();
		/* Request to enter STANDBY mode (Wake Up flag is cleared in PWR_EnterSTANDBYMode function) */
		PWR_EnterSTANDBYMode();
		while(1){} //is not needed since from here the system will wakeup into a reset
	}
	break;
	}

	/* Configure the System Clock to 32MHz */
	//SetHCLKTo32();

	/* Reset lowpower status variable*/
	LowPowerStatus = 0x00;

	RCC->AHBENR = RCC_AHBENR;
	RCC->APB2ENR = RCC_APB2ENR;
	RCC->APB1ENR = RCC_APB1ENR;

	/* Clear Wake Up flag */
	PWR_ClearFlag(PWR_FLAG_WU);

	/* Enable PVD */
	PWR_PVDCmd(ENABLE);
}




void lowPowerSwitchSystemOff( void  )
{
	SET_LED_BLUE();
	SET_LED_RED();
	boardSetPinsToIntput();
	for (int i=0; i<250000; i++);
#if 1
	PWR_RTCAccessCmd(ENABLE);
	RTC_WakeUpCmd(DISABLE);
	PWR_RTCAccessCmd(DISABLE);
#else
	lowPowerInitPeriodicWakeUp(5);
#endif


	lowPowerStartLP(STM32L_STANDBY, RTC_STATE_ON);
}


void RTC_WKUP_IRQHandler(void)
{
	if(RTC_GetITStatus(RTC_IT_WUT) != RESET) {
		portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;


		EXTI_ClearITPendingBit(EXTI_Line20); //OK
		PWR_RTCAccessCmd(ENABLE);
		RTC_ClearITPendingBit(RTC_IT_WUT);
		RTC_ClearFlag(RTC_FLAG_WUTF);
		RTC_WakeUpCmd(DISABLE);

		PWR_RTCAccessCmd(DISABLE);

		xSemaphoreGiveFromISR(semaphoreWakeUp, &xHigherPriorityTaskWoken);

		portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
	}
}

