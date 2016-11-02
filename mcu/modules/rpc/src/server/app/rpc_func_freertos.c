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
#include "main.h"
#include "vc.h"
#include "version_info.h"


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

crystalBoolResult_t mcuInitFirmwareTransfer(firmware_descriptor_t firmwareDescriptor_in[1], uint8_t sha256_in[32], uint8_t aes128_iv_in[16], crystalBoolCrypto_t crypto){
	return programmerInitFirmwareTransfer(firmwareDescriptor_in,sha256_in,aes128_iv_in, crypto);
}


crystalBoolResult_t mcuWriteFirmwareBlock(uint8_t data_in[TRANSMISSION_BLOCK_SIZE]){
	return programmerWriteBlock(data_in,TRANSMISSION_BLOCK_SIZE);
}

crystalBoolResult_t mcuReadFirmwareBlock(uint8_t data_out[TRANSMISSION_BLOCK_SIZE]){
	return programmerReadBlock(data_out,TRANSMISSION_BLOCK_SIZE);
}

crystalBoolResult_t mcuVerifyFirmware(){
	return programmerVerify();
}

firmware_descriptor_t mcuGetFirmwareDescriptor( ){
	return programmerGetFirmwareDescriptor();
}

mcu_descriptor_t mcuGetMCUDescriptor( ){
	return programmerGetMCUDescriptor();
}

device_descriptor_v1_t mcuGetDeviceDescriptor(void){
	device_descriptor_v1_t result;
	uint8_t devName[] = VERSION_INFO_NAME;
	uint8_t versionString[] = VERSION_INFO_VERSION;
	memset(&result,0,sizeof(result));

	result.githash = GITHASH;
	result.gitDate_unix = GITUNIX;
	result.serialnumber = 0;
	programmerGetGUID(result.guid);

	result.deviceID = VERSION_INFO_ID;
	result.boardRevision = 0;
	memcpy(result.name,devName,sizeof(result.name));
	memcpy(result.version,versionString,sizeof(result.version));
	return result;
}

void mcuReboot(void){

}

void mcuEnterProgrammerMode(void){
	mainEnterProgrammingMode();
}

void mcuRunApplication(void){
	programmerRunApplication();
}



