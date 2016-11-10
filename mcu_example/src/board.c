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



const pinGPIO_t gpioPins[] = {PIN_LED_BLUE, PIN_LED_RED};

const pinGPIO_t afPins[] = {PIN_DBG_RX};

void boardPowerOn(void){


	__GPIOA_CLK_ENABLE();
	__GPIOB_CLK_ENABLE();
	__GPIOC_CLK_ENABLE();

	__GPIOA_CLK_SLEEP_ENABLE();
	__GPIOB_CLK_SLEEP_ENABLE();

	__TIM9_CLK_ENABLE();


}

void boardConfigurePIO(void){
	size_t i;

	boardPowerOn();

	for (i=0;i<sizeof(gpioPins)/sizeof(pinGPIO_t);i++){
		boardInitPin(&gpioPins[i]);
	}

	for (i=0;i<sizeof(afPins)/sizeof(pinGPIO_t);i++){
		boardInitAFPin(&afPins[i]);
	}
}

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

void boardInitPin(const pinGPIO_t *pin){
	HAL_GPIO_Init(pin->port, (GPIO_InitTypeDef*) &pin->pinDef);
}

void boardInitAFPin(const pinGPIO_t *pinAF){
	HAL_GPIO_Init(pinAF->port,  (GPIO_InitTypeDef*) &pinAF->pinDef);
}

#if 1
bool boardTestKey(key_id_t keyID){
	switch(keyID){

	case kid_onoff:
			if(GET_KEYONOFF())
				return true;
			else
				return false;
		break;

	case kid_key1:
			if(GET_KEY_1())
				return true;
			else
				return false;
		break;
	case kid_KEYCOUNT:
	case kid_none:
		return false;
	}
	return false;
}
#endif

void boardSetLED(led_id_t ledID, led_state_t ledState){
	switch(ledID){
	case lid_red:
#if 1
		if(ledState == les_on)
			SET_LED_RED();
		else
			CLEAR_LED_RED();
#endif
		break;



	case lid_blue:
#if 1
		if(ledState == les_on)
			SET_LED_BLUE();
		else
			CLEAR_LED_BLUE();
#endif
		break;
	case lid_LEDCOUNT:
	case lid_none:
		break;
	}
}

