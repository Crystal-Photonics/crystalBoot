/*
 * programmer.c
 *
 *  Created on: 18.10.2016
 *      Author: ak
 */
#include <string.h>
#include "bootloader_config.h"
#include "programmer.h"
#include "port_flash.h"
#include "device_id_mapper.h"

#define BLOCK_LENGTH 128
static uint32_t programmWritePointerAddress = APPLICATION_ADDRESS;
static uint32_t programmReadPointerAddress = APPLICATION_ADDRESS;



static firmware_meta_t firmwareMetaData;



#ifndef  BOOTLOADER_BOOT_APP_USING_RESET
#error "please define BOOTLOADER_BOOT_APP_USING_RESET"
#endif

void programmer_init(){
	portFlashReadFirmwareDescriptorBuffer((uint8_t*)&firmwareMetaData,sizeof(firmware_meta_t));
}

crystalBoolResult_t programmerErase(){
	if (portFlashEraseApplication()){
		return crystalBool_OK;
	}else{
		return crystalBool_Fail;
	}
}

crystalBoolResult_t programmerInitFirmwareTransfer(firmware_descriptor_t *firmwareDescriptor){
	if (firmwareDescriptor->entryPoint < MINIMAL_APPLICATION_ADDRESS){
		return crystalBool_Fail;
	}
	uint32_t flashSize = portFlashGetFlashSize();
	if (firmwareDescriptor->entryPoint+firmwareDescriptor->size > flashSize+FLASH_ADDRESS){
		return crystalBool_Fail;
	}

	if ((BOOTLOADER_ALLOW_PAIN_TEXT_COMMUNICATION==0) && (firmwareDescriptor->crypto == crystalBoolCrypto_Plain)){
		return crystalBool_Fail;
	}

	memcpy(&firmwareMetaData.firmwareDescriptor,firmwareDescriptor,sizeof(firmware_descriptor_t));

	programmWritePointerAddress = APPLICATION_ADDRESS;
	programmReadPointerAddress  = APPLICATION_ADDRESS;

	firmwareMetaData.checksumVerified = 0;
	memset(firmwareMetaData.sha256,0,sizeof(firmwareMetaData.sha256));
	portFlashSaveFirmwareDescriptorBuffer((uint8_t*)&firmwareMetaData,sizeof(firmware_meta_t));
	return crystalBool_OK;

}

crystalBoolResult_t programmerWriteBlock(uint8_t *data, size_t size){
	if (portFlashWrite(programmWritePointerAddress, data,  size)){
		if (!portFlashVerifyAgainstBuffer(programmWritePointerAddress, data,  size)){
			return crystalBool_Fail;

		}
		programmWritePointerAddress += size;
		return crystalBool_OK;
	}else{
		return crystalBool_Fail;
	}
}

crystalBoolResult_t programmerReadBlock(uint8_t *data, size_t size){
	if (portFlashRead(programmReadPointerAddress, data,  size)){
		programmReadPointerAddress += size;
		return crystalBool_OK;
	}else{
		return crystalBool_Fail;
	}
}

void programmerRunApplication(void){
#if BOOTLOADER_BOOT_APP_USING_RESET
	portFlashRunApplicationAfterReset();
#else
	portFlashRunApplication();
#endif

}

firmware_descriptor_t programmerGetFirmwareDescriptor( ){
	firmware_descriptor_t result;
	memcpy(&result,&firmwareMetaData.firmwareDescriptor,sizeof(result));
	return result;
}

void programmerGetGUID(uint8_t guid[12]){
	portFlashGetGUID(guid);
}

mcu_descriptor_t programmerGetMCUDescriptor( ){
	mcu_descriptor_t result;
	memset(&result,0,sizeof(result));

	programmerGetGUID(result.guid);
	result.devID = 						portFlashGetDeviceID();
	result.revision = 					portFlashGetRevisionID();
	result.flashsize = 					portFlashGetFlashSize();
	result.firmwareEntryPoint = 		APPLICATION_ADDRESS;
	result.minimalFirmwareEntryPoint = 	MINIMAL_APPLICATION_ADDRESS;
	result.availFlashSize = 			FLASH_ADDRESS+result.flashsize - APPLICATION_ADDRESS;

	return result;
}
