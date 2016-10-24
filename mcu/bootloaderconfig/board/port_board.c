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

#include "port_board.h"



const pinGPIO_t gpioPins[] = { PIN_LED_RED,PIN_LED_BLUE,PIN_KEY_1,PIN_DBG_TX_PIO};


const pinGPIO_t afPins[] = {PIN_DBG_RX		,PIN_DBG_TX
};

void boardDeinit(void){
	/* restore chip to a condition as it was just booting up
	 *
	 */
	for (size_t i=0;i<sizeof(gpioPins)/sizeof(pinGPIO_t);i++){
		GPIO_DeInit(gpioPins[i].port);
	}
	for (size_t i=0;i<sizeof(afPins)/sizeof(pinGPIO_t);i++){
		GPIO_DeInit(afPins[i].port);
	}

}

void boardInit(void){


	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);

	for (size_t i=0;i<sizeof(gpioPins)/sizeof(pinGPIO_t);i++){
		GPIO_Init(gpioPins[i].port, (GPIO_InitTypeDef*) &gpioPins[i].pinDef);
	}

	for (size_t i=0;i<sizeof(afPins)/sizeof(pinGPIO_t);i++){
		GPIO_Init(afPins[i].port, (GPIO_InitTypeDef*) &afPins[i].pinDef);
	}

	for (size_t i=0;i<sizeof(afPins)/sizeof(pinGPIO_t);i++){
		GPIO_PinAFConfig(afPins[i].port, afPins[i].pinSource, afPins[i].af);
	}
}

bool getEnterBootloaderKeyState(){
	return true;
}


