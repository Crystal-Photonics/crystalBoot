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



#include "stm32l1xx_conf.h"
#include <stdint.h>
#include <stdbool.h>

#if 1
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"

#include <queue.h>
#include <semphr.h>
#endif

/** Name of the board */
#define BOARD_NAME "rpcFREERTOS_TEMPLATE"

#define rpcFREERTOS_TEMPLATE

//A:
#define ADC_CHAN_1 ADC_Channel_10
#define ADC_CHAN_2 ADC_Channel_1


typedef struct {
	GPIO_InitTypeDef pinDef;
	GPIO_TypeDef * port;
} pinGPIO_t;

typedef struct {
	GPIO_InitTypeDef pinDef;
	GPIO_TypeDef * port;
	uint16_t pinSource;
	uint8_t af;
} pinAlternateFunction_t;

typedef enum{kid_none,  kid_key1, kid_onoff, kid_KEYCOUNT}key_id_t;
typedef enum{lid_none, lid_red, lid_blue, lid_LEDCOUNT}led_id_t;
typedef enum{les_none, les_on, les_off}led_state_t;


#define PIN_LED_RED   			{.pinDef = {.GPIO_Pin = GPIO_Pin_7, .GPIO_Mode=GPIO_Mode_OUT, .GPIO_Speed= GPIO_Speed_2MHz, .GPIO_OType = GPIO_OType_PP, .GPIO_PuPd=GPIO_PuPd_NOPULL }, .port = GPIOC}
#define SET_LED_RED()			GPIO_SetBits(GPIOC, GPIO_Pin_7)
#define CLEAR_LED_RED()			GPIO_ResetBits(GPIOC, GPIO_Pin_7)

#define PIN_LED_BLUE   			{.pinDef = {.GPIO_Pin = GPIO_Pin_6, .GPIO_Mode=GPIO_Mode_OUT, .GPIO_Speed= GPIO_Speed_2MHz, .GPIO_OType = GPIO_OType_PP, .GPIO_PuPd=GPIO_PuPd_NOPULL }, .port = GPIOC}
#define SET_LED_BLUE()			GPIO_SetBits(GPIOC, GPIO_Pin_6)
#define CLEAR_LED_BLUE()		GPIO_ResetBits(GPIOC, GPIO_Pin_6)

#define PIN_LED_GREEN   			{.pinDef = {.GPIO_Pin = GPIO_Pin_6, .GPIO_Mode=GPIO_Mode_OUT, .GPIO_Speed= GPIO_Speed_2MHz, .GPIO_OType = GPIO_OType_PP, .GPIO_PuPd=GPIO_PuPd_NOPULL }, .port = GPIOC}
#define SET_LED_GREEN()			GPIO_SetBits(GPIOC, GPIO_Pin_6)
#define CLEAR_LED_GREEN()		GPIO_ResetBits(GPIOC, GPIO_Pin_6)

#define PIN_KEY_1   			{.pinDef = {.GPIO_Pin = GPIO_Pin_10, .GPIO_Mode=GPIO_Mode_IN, .GPIO_Speed= GPIO_Speed_2MHz, .GPIO_OType = GPIO_OType_PP, .GPIO_PuPd=GPIO_PuPd_DOWN }, .port = GPIOC}
#define GET_KEY_1()				GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_10)

#define PIN_KEY_ONOFF  			{.pinDef = {.GPIO_Pin = GPIO_Pin_11, .GPIO_Mode=GPIO_Mode_IN, .GPIO_Speed= GPIO_Speed_2MHz, .GPIO_OType = GPIO_OType_PP, .GPIO_PuPd=GPIO_PuPd_DOWN }, .port = GPIOC}
#define GET_KEYONOFF()			GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_11)



#define PIN_DBG_RX 				{.pinSource = GPIO_PinSource10, .pinDef = {.GPIO_Pin = GPIO_Pin_10, .GPIO_Mode=GPIO_Mode_AF, .GPIO_Speed= GPIO_Speed_10MHz, .GPIO_OType = GPIO_OType_PP,.GPIO_PuPd=GPIO_PuPd_NOPULL }, .port = GPIOA, .af=GPIO_AF_USART1}
#define PIN_DBG_TX 				{.pinSource = GPIO_PinSource9,  .pinDef = {.GPIO_Pin = GPIO_Pin_9 , .GPIO_Mode=GPIO_Mode_AF, .GPIO_Speed= GPIO_Speed_10MHz, .GPIO_OType = GPIO_OType_PP,.GPIO_PuPd=GPIO_PuPd_NOPULL }, .port = GPIOA, .af=GPIO_AF_USART1}

#define PIN_PIO_DBG_TX 			{.pinDef = {.GPIO_Pin = GPIO_Pin_9 , .GPIO_Mode=GPIO_Mode_IN, .GPIO_Speed= GPIO_Speed_10MHz, .GPIO_OType = GPIO_OType_PP,.GPIO_PuPd=GPIO_PuPd_NOPULL }, .port = GPIOA}
#define SET_PIO_DBG_TX()		GPIO_SetBits(GPIOA, GPIO_Pin_9)
#define CLEAR_PIO_DBG_TX()		GPIO_ResetBits(GPIOA, GPIO_Pin_9)

#define PIN_SDA   				{.pinSource = GPIO_PinSource11, .pinDef = {.GPIO_Pin = GPIO_Pin_11, .GPIO_Mode=GPIO_Mode_AF, .GPIO_Speed= GPIO_Speed_2MHz, .GPIO_OType = GPIO_OType_PP,.GPIO_PuPd=GPIO_PuPd_NOPULL }, .port = GPIOB, .af=GPIO_AF_I2C2}
#define PIN_SCL   				{.pinSource = GPIO_PinSource10, .pinDef = {.GPIO_Pin = GPIO_Pin_10, .GPIO_Mode=GPIO_Mode_AF, .GPIO_Speed= GPIO_Speed_2MHz, .GPIO_OType = GPIO_OType_PP,.GPIO_PuPd=GPIO_PuPd_NOPULL }, .port = GPIOB, .af=GPIO_AF_I2C2}

#define PIN_PIO_SDA   			{.pinDef = {.GPIO_Pin = GPIO_Pin_11, .GPIO_Mode=GPIO_Mode_OUT, .GPIO_Speed= GPIO_Speed_2MHz, .GPIO_OType = GPIO_OType_PP,.GPIO_PuPd=GPIO_PuPd_NOPULL }, .port = GPIOB}
#define PIN_PIO_SCL   			{.pinDef = {.GPIO_Pin = GPIO_Pin_10, .GPIO_Mode=GPIO_Mode_OUT, .GPIO_Speed= GPIO_Speed_2MHz, .GPIO_OType = GPIO_OType_PP,.GPIO_PuPd=GPIO_PuPd_NOPULL }, .port = GPIOB}




#define PIN_USB_N   			{.pinSource = GPIO_PinSource11, .pinDef = {.GPIO_Pin = GPIO_Pin_11, .GPIO_Mode=GPIO_Mode_AF, .GPIO_Speed= GPIO_Speed_2MHz, .GPIO_OType = GPIO_OType_PP,.GPIO_PuPd=GPIO_PuPd_NOPULL }, .port = GPIOA, .af=GPIO_AF_USB}
#define PIN_USB_P   			{.pinSource = GPIO_PinSource12, .pinDef = {.GPIO_Pin = GPIO_Pin_12, .GPIO_Mode=GPIO_Mode_AF, .GPIO_Speed= GPIO_Speed_2MHz, .GPIO_OType = GPIO_OType_PP,.GPIO_PuPd=GPIO_PuPd_NOPULL }, .port = GPIOA, .af=GPIO_AF_USB}


#define PIN_ADC_1   			{.pinDef = {.GPIO_Pin = GPIO_Pin_1, .GPIO_Mode=GPIO_Mode_AN, .GPIO_Speed= GPIO_Speed_2MHz, .GPIO_OType = GPIO_OType_PP, .GPIO_PuPd=GPIO_PuPd_NOPULL }, .port = GPIOA}
#define PIN_ADC_2   			{.pinDef = {.GPIO_Pin = GPIO_Pin_0, .GPIO_Mode=GPIO_Mode_AN, .GPIO_Speed= GPIO_Speed_2MHz, .GPIO_OType = GPIO_OType_PP, .GPIO_PuPd=GPIO_PuPd_NOPULL }, .port = GPIOC}




#define COM_DBG_BASE                      	USART1
#define COM_DBG_CLK                    		RCC_APB2Periph_USART1


void boardConfigurePIO(void);
void boardPowerOnPIO(void);
void boardInitAFPin(const pinAlternateFunction_t *pinAF);
void boardInitPin(const pinGPIO_t *pin);

void boardSetPinsToIntput(void);
void boardReinitPins(void);

bool boardTestKey(key_id_t keyID);
void boardSetLED(led_id_t ledID, led_state_t ledState);

#endif /* BOARD_H_ */
