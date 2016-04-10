/*
 * rpc_func_arduino.c
 *
 *  Created on: 15.11.2015
 *      Author: arne
 */

#include "../../../include/rpc_transmission/server/app/qt2mcu.h"
#include "board.h"
#include "globals.h"
void mcuSendDataNoAnswer(uint8_t data){

}

uint16_t mcuSetMCUTargetTemperature(uint16_t data){
	static uint8_t ledstatus = 0;
	ledstatus++;
	TARGETEMP_C = data;
	if (ledstatus & 1){
		SET_LED_M();
	}else{
		CLEAR_LED_M();
	}
	(void) data;
	return 0;
}

