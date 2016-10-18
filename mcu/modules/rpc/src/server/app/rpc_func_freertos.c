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


uint8_t mcuEraseFlash(){
	return programmerErase();
}

void mcuResetReadWritePointer(void){
	programmerReset();
}

void mcuWriteFirmwareBlock(uint8_t data_in[128]){
	programmerWriteBlock(data_in,128);
}

void mcuReadFirmwareBlock(uint8_t data_out[128]){
	programmerReadBlock(data_out,128);
}

firmwareVerifyResult_t mcuVerifyFirmware(){
	firmwareVerifyResult_t result = fvr_ERROR;
	return result;
}

firmware_descriptor_t mcuGetFirmwareDescriptor( ){
	firmware_descriptor_t result;
	memset(&result,0,sizeof(result));
	return result;
}

mcu_descriptor_t mcuGetMCUDescriptor( ){
	mcu_descriptor_t result;
	memset(&result,0,sizeof(result));
	return result;
}

void mcuReboot(void){

}

void mcuEnterProgrammerMode(void){

}

void mcuRunApplication(void){

}



