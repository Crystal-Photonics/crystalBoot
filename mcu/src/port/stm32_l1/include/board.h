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

#ifndef STM32L151xE
#define STM32L151xE
#endif


#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>


#include "stm32l1xx_conf.h"
#include "stm32l1xx.h"

/** Name of the board */
#define BOARD_NAME "rpcFREERTOS_TEMPLATE"

/**
  * @brief General Purpose IO
  */
#if 0
typedef struct
{
  __IO uint32_t MODER;        /*!< GPIO port mode register,                     Address offset: 0x00      */
  __IO uint16_t OTYPER;       /*!< GPIO port output type register,              Address offset: 0x04      */
  uint16_t RESERVED0;         /*!< Reserved,                                    0x06                      */
  __IO uint32_t OSPEEDR;      /*!< GPIO port output speed register,             Address offset: 0x08      */
  __IO uint32_t PUPDR;        /*!< GPIO port pull-up/pull-down register,        Address offset: 0x0C      */
  __IO uint16_t IDR;          /*!< GPIO port input data register,               Address offset: 0x10      */
  uint16_t RESERVED1;         /*!< Reserved,                                    0x12                      */
  __IO uint16_t ODR;          /*!< GPIO port output data register,              Address offset: 0x14      */
  uint16_t RESERVED2;         /*!< Reserved,                                    0x16                      */
  __IO uint16_t BSRRL;        /*!< GPIO port bit set/reset low registerBSRR,    Address offset: 0x18      */
  __IO uint16_t BSRRH;        /*!< GPIO port bit set/reset high registerBSRR,   Address offset: 0x1A      */
  __IO uint32_t LCKR;         /*!< GPIO port configuration lock register,       Address offset: 0x1C      */
  __IO uint32_t AFR[2];       /*!< GPIO alternate function low register,        Address offset: 0x20-0x24 */
  __IO uint16_t BRR;          /*!< GPIO bit reset register,                     Address offset: 0x28      */
  uint16_t RESERVED3;         /*!< Reserved,                                    0x2A                      */
} GPIO_TypeDef;



typedef struct
{
  uint32_t GPIO_Pin;              /*!< Specifies the GPIO pins to be configured.
                                       This parameter can be any value of @ref GPIO_pins_define */

  GPIOMode_TypeDef GPIO_Mode;     /*!< Specifies the operating mode for the selected pins.
                                       This parameter can be a value of @ref GPIOMode_TypeDef */

  GPIOSpeed_TypeDef GPIO_Speed;   /*!< Specifies the speed for the selected pins.
                                       This parameter can be a value of @ref GPIOSpeed_TypeDef */

  GPIOOType_TypeDef GPIO_OType;   /*!< Specifies the operating output type for the selected pins.
                                       This parameter can be a value of @ref GPIOOType_TypeDef */

  GPIOPuPd_TypeDef GPIO_PuPd;     /*!< Specifies the operating Pull-up/Pull down for the selected pins.
                                       This parameter can be a value of @ref GPIOPuPd_TypeDef */
}GPIO_InitTypeDef;
#endif

typedef struct {
	GPIO_InitTypeDef pinDef;
	GPIO_TypeDef * port;
} pinGPIO_t;

typedef enum
{
  COM1 = 0,
  COM2 = 1
} COM_TypeDef;


typedef enum{kid_none,  kid_key1, kid_onoff, kid_KEYCOUNT}key_id_t;
typedef enum{lid_none, lid_red, lid_blue, lid_LEDCOUNT}led_id_t;
typedef enum{les_none, les_on, les_off}led_state_t;


#define PIN_LED_RED   			{.pinDef = {.GPIO_Pin = GPIO_Pin_7, .GPIO_Mode=GPIO_Mode_OUT, .GPIO_Speed = GPIO_Speed_2MHz,   .GPIO_PuPd=GPIO_PuPd_NOPULL }, .port = GPIOC}
#define SET_LED_RED()			GPIO_SetBits(GPIOC, GPIO_Pin_7)
#define CLEAR_LED_RED()			GPIO_ResetBits(GPIOC, GPIO_Pin_7)



#define PIN_KEY_1   			{.pinDef = {.GPIO_Pin = GPIO_Pin_10, .GPIO_Mode=GPIO_Mode_IN, .GPIO_Speed = GPIO_Speed_10MHz,   .GPIO_PuPd=GPIO_PuPd_DOWN }, .port = GPIOC}
#define GET_KEY_1()				GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_10)





#define PIN_DBG_RX 				{.pinDef = {.GPIO_Pin = GPIO_Pin_10, .GPIO_Mode=GPIO_Mode_AF, .GPIO_Speed= GPIO_Speed_40MHz,  .GPIO_PuPd=GPIO_PuPd_NOPULL  }, .port = GPIOA}
#define PIN_DBG_TX 				{.pinDef = {.GPIO_Pin = GPIO_Pin_9 , .GPIO_Mode=GPIO_Mode_AF, .GPIO_Speed= GPIO_Speed_40MHz,  .GPIO_PuPd=GPIO_PuPd_NOPULL  },  .port = GPIOA}

//, .Alternate= GPIO_AF7_USART1
#define COM_DBG_BASE                      	USART1
#define COM_DBG_CLK                    		RCC_APB2Periph_USART1


/**
 * @brief Definition for COM port1, connected to USART1
 */
#define EVAL_COM1                        USART1
#define EVAL_COM1_CLK                    RCC_APB2Periph_USART1
#define EVAL_COM1_TX_PIN                 GPIO_Pin_9
#define EVAL_COM1_TX_GPIO_PORT           GPIOA
#define EVAL_COM1_TX_GPIO_CLK            RCC_AHBPeriph_GPIOA
#define EVAL_COM1_RX_PIN                 GPIO_Pin_10
#define EVAL_COM1_RX_GPIO_PORT           GPIOA
#define EVAL_COM1_RX_GPIO_CLK            RCC_AHBPeriph_GPIOA
#define EVAL_COM1_IRQn                   USART1_IRQn

/**
 * @brief Definition for COM port2, connected to USART2 (USART2 pins remapped on GPIOD)
 */
#define EVAL_COM2                        USART2
#define EVAL_COM2_CLK                    RCC_APB1Periph_USART2
#define EVAL_COM2_TX_PIN                 GPIO_Pin_5
#define EVAL_COM2_TX_GPIO_PORT           GPIOD
#define EVAL_COM2_TX_GPIO_CLK            RCC_AHBPeriph_GPIOD
#define EVAL_COM2_RX_PIN                 GPIO_Pin_6
#define EVAL_COM2_RX_GPIO_PORT           GPIOD
#define EVAL_COM2_RX_GPIO_CLK            RCC_AHBPeriph_GPIOD
#define EVAL_COM2_IRQn                   USART2_IRQn


void boardConfigurePIO(void);
void boardPowerOnPIO(void);
void STM_EVAL_COMInit(COM_TypeDef COM, USART_InitTypeDef* USART_InitStruct);

#if 0
void boardInitAFPin(const pinGPIO_t *pinAF);
void boardInitPin(const pinGPIO_t *pin);

void boardSetPinsToIntput(void);
void boardReinitPins(void);

bool boardTestKey(key_id_t keyID);
void boardSetLED(led_id_t ledID, led_state_t ledState);
#endif

#endif /* BOARD_H_ */
