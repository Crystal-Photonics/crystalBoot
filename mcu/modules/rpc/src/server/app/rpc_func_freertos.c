/*
 * rpc_func_arduino.c
 *
 *  Created on: 15.11.2015
 *      Author: arne
 */

#include <string.h>
#include "../../../include/rpc_transmission/server/app/qt2mcu.h"
//#include "board.h"
#include "programmer.h"


uint8_t mcuEraseFlash(){
	return programmerErase();
}

crystalBoolResult_t mcuSetApplicationAddress(uint32_t applicationAddress){
	(void)applicationAddress;
	return crystalBool_OK;
}

uint32_t mcuGetApplicationAddress(void){
	return 0;
}

void mcuResetReadWritePointerToApplicationAddress(void){
	programmerResetReadWritePointerToApplicationAddress();
}

crystalBoolResult_t mcuWriteFirmwareBlock(uint8_t data_in[128]){
	return programmerWriteBlock(data_in,128);
}

crystalBoolResult_t mcuReadFirmwareBlock(uint8_t data_out[128]){
	return programmerReadBlock(data_out,128);
}

firmwareVerifyResult_t mcuVerifyFirmware(){
	firmwareVerifyResult_t result = crystalBool_VerifyError;
	return result;
}

firmware_descriptor_t mcuGetFirmwareDescriptor( ){
	return programmerGetFirmwareDescriptor();
}

mcu_descriptor_t mcuGetMCUDescriptor( ){
	return programmerGetMCUDescriptor();
}

device_descriptor_t mcuGetDeviceDescriptor(void){
	device_descriptor_t result;
	memset(&result,0,sizeof(result));
	return result;
}

void mcuReboot(void){

}

void mcuEnterProgrammerMode(void){

}

void mcuRunApplication(void){
	programmerRunApplication();
}



