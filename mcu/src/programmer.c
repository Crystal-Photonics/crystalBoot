/*
 * programmer.c
 *
 *  Created on: 18.10.2016
 *      Author: ak
 */
#include <string.h>
#include <assert.h>
#include "bootloader_config.h"
#include "programmer.h"
#include "port_flash.h"
#include "device_id_mapper.h"
#include "channel_codec/crc16.h"
#include "sha256.h"

SHA256_CTX sha56_ctx;

#define BLOCK_LENGTH 128
static uint32_t programmWritePointerAddress = APPLICATION_ADDRESS;
static uint32_t programmReadPointerAddress = APPLICATION_ADDRESS;



static firmware_meta_t firmwareMetaData;
static bool firmware_meta_is_valid = false;


#ifndef  BOOTLOADER_BOOT_APP_USING_RESET
#error "please define BOOTLOADER_BOOT_APP_USING_RESET"
#endif


static void programmer_destroyMetaData(){
#if 1
	memset(&firmwareMetaData,(uint8_t) 0x00,sizeof firmwareMetaData );
	firmwareMetaData.d.checksumVerified = 0;
	assert(portFlashSaveFirmwareDescriptorBuffer((uint8_t*)&firmwareMetaData,sizeof firmwareMetaData ));
#endif
	firmware_meta_is_valid = false;
}

static void programer_readMetaData(){
	assert(portFlashReadFirmwareDescriptorBuffer((uint8_t*)&firmwareMetaData,sizeof(firmwareMetaData)));

	uint16_t calcedCRC = crc16_buffer((uint8_t*) &firmwareMetaData.d, sizeof(firmwareMetaData.d));
	if (firmwareMetaData.crc16OfMetaData != calcedCRC){
		programmer_destroyMetaData();
		firmware_meta_is_valid = false;

	}else{
		firmware_meta_is_valid = true;
	}
}

static void programer_writeMetaData(){
#if 1
	firmwareMetaData.crc16OfMetaData = crc16_buffer((uint8_t*)&firmwareMetaData.d, sizeof firmwareMetaData.d);
	assert(portFlashSaveFirmwareDescriptorBuffer((uint8_t*)&firmwareMetaData,sizeof(firmwareMetaData)));
	firmware_meta_is_valid = true;
#endif
}

void programmer_init(){
	programer_readMetaData();
}

crystalBoolResult_t programmerErase(){
#if 1
	programmer_destroyMetaData();
	if (portFlashEraseApplication()){
		return crystalBool_OK;
	}else{
		return crystalBool_Fail;
	}
#else
	return crystalBool_Fail;
#endif
}

crystalBoolResult_t programmerVerify(void){

	crystalBoolResult_t result = crystalBool_OK;
#if 1
	if (!firmware_meta_is_valid){
		return crystalBool_Fail;
	}
#define FLASH_BLOCK_BUFFER_SIZE 32

	uint8_t sha256_calced_hash[SHA256_BLOCK_SIZE];
	uint8_t flash_bloc_buffer[FLASH_BLOCK_BUFFER_SIZE];

	sha256_init(&sha56_ctx);
	uint32_t pos=0;
	while(pos < firmwareMetaData.d.firmwareDescriptor.size){
		uint32_t size = FLASH_BLOCK_BUFFER_SIZE;
		if (pos+size > firmwareMetaData.d.firmwareDescriptor.size){
			size = firmwareMetaData.d.firmwareDescriptor.size - pos;
		}
		portFlashRead(APPLICATION_ADDRESS+pos, flash_bloc_buffer, size);
		sha256_update(&sha56_ctx,flash_bloc_buffer,size);
		pos += size;
	}

	sha256_final(&sha56_ctx,sha256_calced_hash);

#if CHECKSUM_SIZE != FLASH_BLOCK_BUFFER_SIZE
#error "both sha256 lengths should be the same"
#endif

	if (memcmp(sha256_calced_hash,firmwareMetaData.d.sha256,CHECKSUM_SIZE) == 0){
		firmwareMetaData.d.checksumVerified = 1;
	}else{
		firmwareMetaData.d.checksumVerified = 0;
		result = crystalBool_Fail;
	}

	if (result == crystalBool_OK){
		firmwareMetaData.d.checksumVerified = 1;
	}
	programer_writeMetaData();
#endif
	return result;
}

crystalBoolResult_t programmerQuickVerify(void){
	if (!firmware_meta_is_valid){
		return crystalBool_Fail;
	}
	if (firmwareMetaData.d.checksumVerified == 1){
		return crystalBool_OK;
	}else{
		return programmerVerify();
	}
}

crystalBoolResult_t programmerInitFirmwareTransfer(firmware_descriptor_t *firmwareDescriptor, uint8_t sha256[32], const crystalBoolCrypto_t crypto){
	if (firmwareDescriptor->entryPoint < MINIMAL_APPLICATION_ADDRESS){
		return crystalBool_Fail;
	}

	uint32_t flashSize = portFlashGetFlashSize();
	if (firmwareDescriptor->entryPoint+firmwareDescriptor->size > flashSize+FLASH_ADDRESS){
		return crystalBool_Fail;
	}

	if (firmwareDescriptor->entryPoint  != APPLICATION_ADDRESS){
		return crystalBool_Fail;
	}

	if ((BOOTLOADER_ALLOW_PAIN_TEXT_COMMUNICATION==0) && (crypto == crystalBoolCrypto_Plain)){
		return crystalBool_Fail;
	}

	memcpy(&firmwareMetaData.d.firmwareDescriptor,firmwareDescriptor,sizeof(firmware_descriptor_t));

	programmWritePointerAddress = APPLICATION_ADDRESS;
	programmReadPointerAddress  = APPLICATION_ADDRESS;

	firmwareMetaData.d.checksumVerified = 0;
	memcpy(firmwareMetaData.d.sha256,sha256,sizeof(firmwareMetaData.d.sha256));
	programer_writeMetaData();
	return crystalBool_OK;
}

firmware_descriptor_t programmerGetFirmwareDescriptor( ){
	firmware_descriptor_t result;
	memcpy(&result,&firmwareMetaData.d.firmwareDescriptor,sizeof(result));
	return result;
}

crystalBoolResult_t programmerWriteBlock(uint8_t *data, size_t size){
#if 1
	//programmer_decode_block(programmWritePointerAddress, data,  &size);

	if (portFlashWrite(programmWritePointerAddress, data,  size)){
		if (!portFlashVerifyAgainstBuffer(programmWritePointerAddress, data,  size)){
			return crystalBool_Fail;

		}
		programmWritePointerAddress += size;
		return crystalBool_OK;
	}else{
		return crystalBool_Fail;
	}
#else
	return crystalBool_Fail;
#endif
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
	result.cryptoRequired = 			BOOTLOADER_ALLOW_PAIN_TEXT_COMMUNICATION==0;
	return result;
}
//(void)programmer_destroyMetaData();
