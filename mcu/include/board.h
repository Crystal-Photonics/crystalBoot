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

#if 1
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"

#include <queue.h>
#include <semphr.h>
#endif

#include "stm32l1xx_hal.h"
#include "chip_init.h"

/** Name of the board */
#define BOARD_NAME "rpcFREERTOS_TEMPLATE"

#define rpcFREERTOS_TEMPLATE

//A:
#define ADC_CHAN_1 ADC_CHANNEL_10
#define ADC_CHAN_2 ADC_CHANNEL_1


typedef struct {
	GPIO_InitTypeDef pinDef;
	GPIO_TypeDef * port;
} pinGPIO_t;
/*
typedef struct {
	GPIO_InitTypeDef pinDef;
	GPIO_TypeDef * port;
	uint16_t pinSource;
	uint8_t af;
} pinAlternateFunction_t;
*/


typedef enum{kid_none,  kid_key1, kid_onoff, kid_KEYCOUNT}key_id_t;
typedef enum{lid_none, lid_red, lid_blue, lid_LEDCOUNT}led_id_t;
typedef enum{les_none, les_on, les_off}led_state_t;


#define PIN_LED_RED   			{.pinDef = {.Pin = GPIO_PIN_7, .Mode=GPIO_MODE_OUTPUT_PP, .Speed = GPIO_SPEED_FREQ_MEDIUM,   .Pull=GPIO_NOPULL }, .port = GPIOC}
#define SET_LED_RED()			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7,GPIO_PIN_SET)
#define CLEAR_LED_RED()			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7,GPIO_PIN_RESET)

#define PIN_LED_BLUE   			{.pinDef = {.Pin = GPIO_PIN_6, .Mode=GPIO_MODE_OUTPUT_PP, .Speed = GPIO_SPEED_FREQ_MEDIUM , .Pull=GPIO_NOPULL }, .port = GPIOC}
#define SET_LED_BLUE()			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6,GPIO_PIN_SET)
#define CLEAR_LED_BLUE()		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6,GPIO_PIN_RESET)

#define PIN_LED_GREEN   			{.pinDef = {.Pin = GPIO_PIN_6, .Mode=GPIO_MODE_OUTPUT_PP, .Speed = GPIO_SPEED_FREQ_MEDIUM,  .Pull=GPIO_NOPULL  }, .port = GPIOC}
#define SET_LED_GREEN()			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6,GPIO_PIN_SET)
#define CLEAR_LED_GREEN()		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6,GPIO_PIN_RESET)

#define PIN_KEY_1   			{.pinDef = {.Pin = GPIO_PIN_10, .Mode=GPIO_MODE_INPUT, .Speed = GPIO_SPEED_FREQ_MEDIUM,   .Pull=GPIO_PULLDOWN }, .port = GPIOC}
#define GET_KEY_1()				HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_10)

#define PIN_KEY_ONOFF  			{.pinDef = {.Pin = GPIO_PIN_11, .Mode=GPIO_MODE_INPUT, .Speed = GPIO_SPEED_FREQ_MEDIUM,   .Pull=GPIO_PULLDOWN }, .port = GPIOC}
#define GET_KEYONOFF()			HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_11)



#define PIN_DBG_RX 				{.pinDef = {.Pin = GPIO_PIN_10, .Mode=GPIO_MODE_AF_PP, .Speed= GPIO_SPEED_FREQ_HIGH,  .Pull=GPIO_NOPULL  , .Alternate= GPIO_AF7_USART1}, .port = GPIOA}
#define PIN_DBG_TX 				{.pinDef = {.Pin = GPIO_PIN_9 , .Mode=GPIO_MODE_AF_PP, .Speed= GPIO_SPEED_FREQ_HIGH,  .Pull=GPIO_NOPULL , .Alternate= GPIO_AF7_USART1 },  .port = GPIOA}

#define PIN_PIO_DBG_TX 			{.pinDef = {.Pin = GPIO_PIN_9 , .Mode=GPIO_MODE_INPUT, .Speed= GPIO_SPEED_FREQ_HIGH,  .Pull=GPIO_PULLUP , .Alternate= GPIO_AF4_I2C2 }, .port = GPIOA}
#define SET_PIO_DBG_TX()		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9,GPIO_PIN_SET)
#define CLEAR_PIO_DBG_TX()		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9,GPIO_PIN_RESET)

#define PIN_SDA   				{.pinDef = {.Pin = GPIO_PIN_11, .Mode=GPIO_MODE_AF_OD, .Speed = GPIO_SPEED_FREQ_MEDIUM,  .Pull=GPIO_NOPULL, .Alternate= GPIO_AF4_I2C2 }, .port = GPIOB}
#define PIN_SCL   				{.pinDef = {.Pin = GPIO_PIN_10, .Mode=GPIO_MODE_AF_OD, .Speed = GPIO_SPEED_FREQ_MEDIUM , .Pull=GPIO_NOPULL, .Alternate= GPIO_AF4_I2C2 }, .port = GPIOB}

#define PIN_PIO_SDA   			{.pinDef = {.Pin = GPIO_PIN_11, .Mode=GPIO_MODE_OUTPUT_OD, .Speed = GPIO_SPEED_FREQ_MEDIUM,  .Pull=GPIO_NOPULL  }, .port = GPIOB}
#define PIN_PIO_SCL   			{.pinDef = {.Pin = GPIO_PIN_10, .Mode=GPIO_MODE_OUTPUT_OD, .Speed = GPIO_SPEED_FREQ_MEDIUM , .Pull=GPIO_NOPULL  }, .port = GPIOB}




#define PIN_USB_N   			{.pinDef = {.Pin = GPIO_PIN_11, .Mode=GPIO_MODE_AF_PP, .Speed = GPIO_SPEED_FREQ_MEDIUM,  .Pull=GPIO_NOPULL  }, .port = GPIOA }
#define PIN_USB_P   			{.pinDef = {.Pin = GPIO_PIN_12, .Mode=GPIO_MODE_AF_PP, .Speed = GPIO_SPEED_FREQ_MEDIUM,  .Pull=GPIO_NOPULL  }, .port = GPIOA}


#define PIN_ADC_1   			{.pinDef = {.Pin = GPIO_PIN_1, .Mode=GPIO_MODE_ANALOG, .Speed = GPIO_SPEED_FREQ_MEDIUM,   .Pull=GPIO_NOPULL  }, .port = GPIOA}
#define PIN_ADC_2   			{.pinDef = {.Pin = GPIO_PIN_0, .Mode=GPIO_MODE_ANALOG, .Speed = GPIO_SPEED_FREQ_MEDIUM ,  .Pull=GPIO_NOPULL  }, .port = GPIOC}




#define COM_DBG_BASE                      	USART1
#define COM_DBG_CLK                    		RCC_APB2Periph_USART1


void boardConfigurePIO(void);
void boardPowerOnPIO(void);
void boardInitAFPin(const pinGPIO_t *pinAF);
void boardInitPin(const pinGPIO_t *pin);

void boardSetPinsToIntput(void);
void boardReinitPins(void);

bool boardTestKey(key_id_t keyID);
void boardSetLED(led_id_t ledID, led_state_t ledState);

#endif /* BOARD_H_ */
