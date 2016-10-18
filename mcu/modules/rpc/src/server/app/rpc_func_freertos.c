/*
 * rpc_func_arduino.c
 *
 *  Created on: 15.11.2015
 *      Author: arne
 */

#include <string.h>
#include "../../../include/rpc_transmission/server/app/qt2mcu.h"
#include "board.h"
#include "programmer.h"


void mcuResetReadWritePointer(void){
	programmerReset();
}

void mcuWriteFirmwareBlock(uint8_t data_in[32]){
	programmerWriteBlock(data_in,32);
}

void mcuReadFirmwareBlock(uint8_t data_out[32]){
	programmerReadBlock(data_out,32);
}

firmwareVerifyResult_t mcuVerifyFirmware(){
	firmwareVerifyResult_t result = fvr_ERROR;
	return result;
}

firmware_descriptor_t mcuGetFirmwareDescriptor( ){
	firmware_descriptor_t result;
	memset(&result,sizeof(result),0);
	return result;
}

mcu_descriptor_t mcuGetMCUDescriptor( ){
	mcu_descriptor_t result;
	memset(&result,sizeof(result),0);
	return result;
}

void mcuReboot(void){

}

