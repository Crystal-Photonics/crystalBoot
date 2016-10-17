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



const pinGPIO_t gpioPins[] = { PIN_LED_RED,PIN_KEY_1};

const pinGPIO_t afPins[] = {PIN_DBG_RX,PIN_DBG_TX};

const uint32_t COM_TX_PORT_CLK[2] = {EVAL_COM1_TX_GPIO_CLK, EVAL_COM2_TX_GPIO_CLK};

const uint32_t COM_RX_PORT_CLK[2] = {EVAL_COM1_RX_GPIO_CLK, EVAL_COM2_RX_GPIO_CLK};

const uint32_t COM_USART_CLK[2] = {EVAL_COM1_CLK, EVAL_COM2_CLK};

const uint16_t COM_TX_PIN[2] = {EVAL_COM1_TX_PIN, EVAL_COM2_TX_PIN};

const uint16_t COM_RX_PIN[2] = {EVAL_COM1_RX_PIN, EVAL_COM2_RX_PIN};

GPIO_TypeDef* COM_TX_PORT[2] = {EVAL_COM1_TX_GPIO_PORT, EVAL_COM2_TX_GPIO_PORT};

GPIO_TypeDef* COM_RX_PORT[2] = {EVAL_COM1_RX_GPIO_PORT, EVAL_COM2_RX_GPIO_PORT};

USART_TypeDef* COM_USART[2] = {EVAL_COM1, EVAL_COM2};

void boardPowerOn(void){

#if 0
	__GPIOA_CLK_ENABLE();
	__GPIOB_CLK_ENABLE();
	__GPIOC_CLK_ENABLE();
#endif


}

/**
  * @brief  Configures COM port.
  * @param  COM: Specifies the COM port to be configured.
  *   This parameter can be one of following parameters:
  *     @arg COM1
  *     @arg COM2
  * @param  USART_InitStruct: pointer to a USART_InitTypeDef structure that
  *   contains the configuration information for the specified USART peripheral.
  * @retval None
  */
void STM_EVAL_COMInit(COM_TypeDef COM, USART_InitTypeDef* USART_InitStruct)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Enable GPIO clock */
  RCC_APB2PeriphClockCmd(COM_TX_PORT_CLK[COM] | COM_RX_PORT_CLK[COM], ENABLE);


  /* Enable UART clock */
  if (COM == COM1)
  {
    RCC_APB2PeriphClockCmd(COM_USART_CLK[COM], ENABLE);
  }
  else
  {
    /* Enable the USART2 Pins Software Remapping */
    //GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);
    RCC_APB1PeriphClockCmd(COM_USART_CLK[COM], ENABLE);
  }

  /* Configure USART Tx as alternate function push-pull */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Pin = COM_TX_PIN[COM];
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_Init(COM_TX_PORT[COM], &GPIO_InitStructure);

  /* Configure USART Rx as input floating */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_Pin = COM_RX_PIN[COM];
  GPIO_Init(COM_RX_PORT[COM], &GPIO_InitStructure);

  /* USART configuration */
  USART_Init(COM_USART[COM], USART_InitStruct);

  /* Enable USART */
  USART_Cmd(COM_USART[COM], ENABLE);
}

void boardInitPin(const pinGPIO_t *pin){
	GPIO_Init(pin->port, (GPIO_InitTypeDef*) &pin->pinDef);
}

void boardInitAFPin(const pinGPIO_t *pinAF){
	//HAL_GPIO_Init(pinAF->port,  (GPIO_InitTypeDef*) &pinAF->pinDef);
}


void boardConfigurePIO(void){
	size_t i;

#if 0
	//void STM_EVAL_PBInit(Button_TypeDef Button, ButtonMode_TypeDef Button_Mode)
	{
	  GPIO_InitTypeDef GPIO_InitStructure;
	  EXTI_InitTypeDef EXTI_InitStructure;
	  NVIC_InitTypeDef NVIC_InitStructure;

	  /* Enable the BUTTON Clock */
	  RCC_APB2PeriphClockCmd(BUTTON_CLK[Button] | RCC_APB2Periph_AFIO, ENABLE);

	  /* Configure Button pin as input floating */
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	  GPIO_InitStructure.GPIO_Pin = BUTTON_PIN[Button];
	  GPIO_Init(BUTTON_PORT[Button], &GPIO_InitStructure);
#endif
	boardPowerOn();

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

	for (i=0;i<sizeof(gpioPins)/sizeof(pinGPIO_t);i++){
		boardInitPin(&gpioPins[i]);

	}

	for (i=0;i<sizeof(afPins)/sizeof(pinGPIO_t);i++){
		boardInitAFPin(&afPins[i]);
	}
}

#if 0
void boardSetPinsToIntput(void){
	//for shutting off vcc power rail it might be necessary to shut down some gpios.
	//especially when these IOs are connected indirectly via protection diodes or Pull ups to VCC.
	//If these Pins are input pins, they cant supply vcc with these indirect connections

	const pinGPIO_t gpio_pins_toinput[]={PIN_PIO_DBG_TX};

	for (size_t i=0;i<sizeof(gpio_pins_toinput)/sizeof(pinGPIO_t);i++){
		boardInitPin(&gpio_pins_toinput[i]);
		HAL_GPIO_WritePin(gpio_pins_toinput[i].port, gpio_pins_toinput[i].pinDef.Pin,RESET);
	}

}

void boardReinitPins(void){
	const pinGPIO_t gpio_pins_tonormal[]={PIN_DBG_TX,PIN_SDA,PIN_SCL};

	for (size_t i=0;i<sizeof(gpio_pins_tonormal)/sizeof(pinGPIO_t);i++){
		boardInitAFPin(&gpio_pins_tonormal[i]);
	}

}
#endif



