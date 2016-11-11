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

#ifndef BOARD_H_
#define BOARD_H_

#if 0
#ifndef STM32L151xE
#define STM32L151xE
#endif
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "stm32l1xx_conf.h"
#include "stm32l1xx.h"


//#define printf(...) (void)0

/** Name of the board */
#define BOARD_NAME "rpcFREERTOS_TEMPLATE"

/**
  * @brief General Purpose IO
  */



typedef struct {
	GPIO_InitTypeDef pinDef;
	GPIO_TypeDef * port;
	uint8_t af;
	uint16_t pinSource;
} pinGPIO_t;




typedef enum{kid_none,  kid_key1, kid_onoff, kid_KEYCOUNT}key_id_t;
typedef enum{lid_none, lid_red, lid_blue, lid_LEDCOUNT}led_id_t;
typedef enum{les_none, les_on, les_off}led_state_t;


#define PIN_LED_RED   			{.pinDef = {.GPIO_Pin = GPIO_Pin_7, .GPIO_Mode=GPIO_Mode_OUT, .GPIO_Speed = GPIO_Speed_2MHz, .GPIO_OType=GPIO_OType_PP, .GPIO_PuPd=GPIO_PuPd_NOPULL }, .port = GPIOC}
#define SET_LED_RED_()			GPIO_SetBits(GPIOC, GPIO_Pin_7)
#define CLEAR_LED_RED_()			GPIO_ResetBits(GPIOC, GPIO_Pin_7)

#define SET_LED_BUSY() 			SET_LED_RED_()
#define CLEAR_LED_BUSY()		CLEAR_LED_RED_()

#define PIN_LED_BLUE   			{.pinDef = {.GPIO_Pin = GPIO_Pin_6, .GPIO_Mode=GPIO_Mode_OUT, .GPIO_Speed = GPIO_Speed_2MHz, .GPIO_OType=GPIO_OType_PP, .GPIO_PuPd=GPIO_PuPd_NOPULL }, .port = GPIOC}
#define SET_LED_BLUE()			GPIO_SetBits(GPIOC, GPIO_Pin_6)
#define CLEAR_LED_BLUE()			GPIO_ResetBits(GPIOC, GPIO_Pin_6)

#define SET_LED_OK() 			SET_LED_BLUE()
#define CLEAR_LED_OK() 			CLEAR_LED_BLUE()

#define PIN_DBG_TX_PIO 			{.pinDef = {.GPIO_Pin = GPIO_Pin_9, .GPIO_Mode=GPIO_Mode_OUT, .GPIO_Speed = GPIO_Speed_40MHz, .GPIO_OType=GPIO_OType_PP,  .GPIO_PuPd=GPIO_PuPd_NOPULL }, .port = GPIOA}
#define SET_DBG_TX_PIO()			GPIO_SetBits(GPIOA, GPIO_Pin_9)
#define CLEAR_DBG_TX_PIO()			GPIO_ResetBits(GPIOA, GPIO_Pin_9)


#define PIN_KEY_1   			{.pinDef = {.GPIO_Pin = GPIO_Pin_10, .GPIO_Mode=GPIO_Mode_IN, .GPIO_Speed = GPIO_Speed_10MHz, .GPIO_OType=GPIO_OType_PP,  .GPIO_PuPd=GPIO_PuPd_DOWN }, .port = GPIOC}
#define GET_KEY_1()				GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_10)


#define PIN_DBG_RX 				{.pinDef = {.GPIO_Pin = GPIO_Pin_10, .GPIO_Mode=GPIO_Mode_AF, .GPIO_Speed= GPIO_Speed_40MHz, .GPIO_OType=GPIO_OType_OD, .GPIO_PuPd=GPIO_PuPd_NOPULL  }, .pinSource=GPIO_PinSource10, .port = GPIOA, .af=GPIO_AF_USART1}
#define PIN_DBG_TX 				{.pinDef = {.GPIO_Pin = GPIO_Pin_9 , .GPIO_Mode=GPIO_Mode_AF, .GPIO_Speed= GPIO_Speed_40MHz, .GPIO_OType=GPIO_OType_PP, .GPIO_PuPd=GPIO_PuPd_NOPULL  }, .pinSource=GPIO_PinSource9, .port = GPIOA,  .af=GPIO_AF_USART1}


#define COM_USART_BASE			USART1
#define COM_USART_CLK			RCC_APB2Periph_USART1
#define COM_USART_IRQ			USART1_IRQn


void boardInit(void);
void boardDeinit(void);
bool getEnterBootloaderKeyState(void);




#endif /* BOARD_H_ */
