/*
 * rpc_func_arduino.c
 *
 *  Created on: 15.11.2015
 *      Author: arne
 */

#include "../../../include/rpc_transmission/server/app/qt2mcu.h"
#include "board.h"

void mcuSendDataNoAnswer(uint8_t data){

}

uint16_t mcuSetLEDStatus(rpcLEDStatus_t ledStatus){

	static uint16_t returnvalue = 0;
	returnvalue++;


	return returnvalue;
}

