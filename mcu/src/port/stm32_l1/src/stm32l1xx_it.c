/**
  ******************************************************************************
  * @file    USART/USART_TwoBoards/DataExchangeInterrupt/stm32l1xx_it.c
  * @author  MCD Application Team
  * @version V1.2.1
  * @date    20-April-2015
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/


#include "stm32l1xx_it.h"
#include <stdint.h>
#include "main.h"

//uint32_t sysTick_ms=0;
/** @addtogroup STM32L1xx_StdPeriph_Examples
* @{
*/

/** @addtogroup USART_DataExchangeInterrupt
* @{
*/





/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/
void **HARDFAULT_PSP;
//static register void *stack_pointer asm("sp");



/* The prototype shows it is a naked function - in effect this is just an
assembly function. */
void HardFault_Handler( void ) __attribute__( ( naked ) );

/* The fault handler implementation calls a function called
prvGetRegistersFromStack(). */
 void HardFault_Handler(void)
{
#if 	 1
    __asm volatile
    (
        " tst lr, #4                                                \n"
        " ite eq                                                    \n"
        " mrseq r0, msp                                             \n"
        " mrsne r0, psp                                             \n"
        " ldr r1, [r0, #24]                                         \n"
        " ldr r2, handler2_address_const                            \n"
        " bx r2                                                     \n"
        " handler2_address_const: .word prvGetRegistersFromStack    \n"
    );
#else
    // Hijack the process stack pointer to make backtrace work
    register void *stack_pointer asm("sp");

    asm("mrs %0, psp" : "=r"(HARDFAULT_PSP) : :);
    stack_pointer = HARDFAULT_PSP;
    (void)stack_pointer;
    while(1);
#endif

}




void prvGetRegistersFromStack( uint32_t *pulFaultStackAddress )
{
/* These are volatile to try and prevent the compiler/linker optimising them
away as the variables never actually get used.  If the debugger won't show the
values of the variables, make them global my moving their declaration outside
of this function. */
volatile uint32_t r0;
volatile uint32_t r1;
volatile uint32_t r2;
volatile uint32_t r3;
volatile uint32_t r12;
volatile uint32_t lr; /* Link register. */
volatile uint32_t pc; /* Program counter. */
volatile uint32_t psr;/* Program status register. */

    r0 = pulFaultStackAddress[ 0 ];
    r1 = pulFaultStackAddress[ 1 ];
    r2 = pulFaultStackAddress[ 2 ];
    r3 = pulFaultStackAddress[ 3 ];

    r12 = pulFaultStackAddress[ 4 ];
    lr = pulFaultStackAddress[ 5 ];
    pc = pulFaultStackAddress[ 6 ];
    psr = pulFaultStackAddress[ 7 ];

    /* When the following line is hit, the variables contain the register values. */
    for( ;; );
    (void)r0;
    (void)r1;
    (void)r2;
    (void)r3;
    (void)r12;
    (void)lr;
    (void)pc;
    (void)psr;

}

/**
* @brief  This function handles NMI exception.
* @param  None
* @retval None
*/
void NMI_Handler(void)
{}



/**
* @brief  This function handles Memory Manage exception.
* @param  None
* @retval None
*/
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {}
}

/**
* @brief  This function handles Bus Fault exception.
* @param  None
* @retval None
*/
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {}
}

/**
* @brief  This function handles Usage Fault exception.
* @param  None
* @retval None
*/
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {}
}

/**
* @brief  This function handles SVCall exception.
* @param  None
* @retval None
*/
void SVC_Handler(void)
{}

/**
* @brief  This function handles Debug Monitor exception.
* @param  None
* @retval None
*/
void DebugMon_Handler(void)
{}

/**
* @brief  This function handles PendSV_Handler exception.
* @param  None
* @retval None
*/
void PendSV_Handler(void)
{}

/**
* @brief  This function handles SysTick Handler.
* @param  None
* @retval None
*/
void SysTick_Handler(void)
{
	sysTick_ms++;
}


void WWDG_IRQHandler  (){ }
void PVD_IRQHandler  (){ }
void TAMPER_STAMP_IRQHandler  (){ }
void RTC_WKUP_IRQHandler  (){ }
void FLASH_IRQHandler  (){ }
void RCC_IRQHandler  (){ }
void EXTI0_IRQHandler  (){ }
void EXTI1_IRQHandler  (){ }
void EXTI2_IRQHandler  (){ }
void EXTI3_IRQHandler  (){ }
void EXTI4_IRQHandler  (){ }
void DMA1_Channel1_IRQHandler  (){ }
void DMA1_Channel2_IRQHandler  (){ }
void DMA1_Channel3_IRQHandler  (){ }
void DMA1_Channel4_IRQHandler  (){ }
void DMA1_Channel5_IRQHandler  (){ }
void DMA1_Channel6_IRQHandler  (){ }
void DMA1_Channel7_IRQHandler  (){ }
void ADC1_IRQHandler  (){ }
void USB_HP_IRQHandler  (){ }
void USB_LP_IRQHandler  (){ }
void DAC_IRQHandler  (){ }
void COMP_IRQHandler  (){ }
void EXTI9_5_IRQHandler  (){ }
void LCD_IRQHandler  (){ }
void TIM9_IRQHandler  (){ }
void TIM10_IRQHandler  (){ }
void TIM11_IRQHandler  (){ }
void TIM2_IRQHandler  (){ }
void TIM3_IRQHandler  (){ }
void TIM4_IRQHandler  (){ }
void I2C1_EV_IRQHandler  (){ }
  void I2C1_ER_IRQHandler  (){ }
  void I2C2_EV_IRQHandler  (){ }
  void I2C2_ER_IRQHandler  (){ }
  void SPI1_IRQHandler  (){ }
  void SPI2_IRQHandler  (){ }


  void EXTI15_10_IRQHandler  (){ }
  void RTC_Alarm_IRQHandler  (){ }
  void USB_FS_WKUP_IRQHandler  (){ }
  void TIM6_IRQHandler  (){ }
  void TIM7_IRQHandler  (){ }

  void TIM5_IRQHandler  (){ }
  void SPI3_IRQHandler  (){ }
  void UART4_IRQHandler  (){ }
  void UART5_IRQHandler  (){ }
  void DMA2_Channel1_IRQHandler  (){ }
  void DMA2_Channel2_IRQHandler  (){ }
  void DMA2_Channel3_IRQHandler  (){ }
  void DMA2_Channel4_IRQHandler  (){ }
  void DMA2_Channel5_IRQHandler  (){ }
  void AES_IRQHandler  (){ }
  void COMP_ACQ_IRQHandler  (){ }
  void BootRAM(){

  }
/******************************************************************************/
/*                 STM32L1xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32l1xx_xx.s).                                            */
/******************************************************************************/
/**
* @brief  This function handles PPP interrupt request.
* @param  None
* @retval None
*/
/*void PPP_IRQHandler(void)
{
}*/

/**
* @}
*/

/**
* @}
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
