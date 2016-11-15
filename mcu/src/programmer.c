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
#include "aes.h"
#include "aes_128_key.h"

SHA256_CTX sha56_ctx;
#define AES_128_IV_LENGTH 16

static uint32_t programmWritePointerAddress = 0;
static uint32_t programmReadPointerAddress = 0;

static uint8_t aes128_iv_buf[AES_128_IV_LENGTH];
static uint8_t aes128_key_local[AES_128_IV_LENGTH];
static uint8_t *pointerToAESkey;
static uint8_t *pointerToAESiv;
static bool transferIsInitialized = false;

static crystalBoolCrypto_t cryptoUsed;
static firmware_meta_t firmwareMetaData;
static bool firmware_meta_is_valid = false;

static uint16_t programmer_AES_reinitialization_wait_time_s;


#ifndef  BOOTLOADER_BOOT_APP_USING_RESET
#error "please define BOOTLOADER_BOOT_APP_USING_RESET"
#endif

uint32_t programmerGetApplicationEntryPoint(){
	return firmwareMetaData.d.firmwareDescriptor.entryPoint;
}

void programmerIncrementAESReInitWaitTime_s(void){
	if (programmer_AES_reinitialization_wait_time_s < 0xFFFF){
		programmer_AES_reinitialization_wait_time_s++;
	}
}

static void programmer_destroyMetaData(){
#if 1
	transferIsInitialized = false;
	memset(&firmwareMetaData,(uint8_t) 0x00,sizeof firmwareMetaData );
	firmwareMetaData.d.checksumVerified = 0;
	firmwareMetaData.d.checksumVerifiedByCrypto = 0;
	assert(portFlashSaveFirmwareDescriptorBuffer((uint8_t*)&firmwareMetaData,sizeof firmwareMetaData ));
#endif
	firmware_meta_is_valid = false;
}

static void programer_readMetaData(){
	transferIsInitialized = false;
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
	transferIsInitialized = false;
	firmwareMetaData.crc16OfMetaData = crc16_buffer((uint8_t*)&firmwareMetaData.d, sizeof firmwareMetaData.d);
	assert(portFlashSaveFirmwareDescriptorBuffer((uint8_t*)&firmwareMetaData,sizeof(firmwareMetaData)));
	firmware_meta_is_valid = true;
#endif
}

void programmer_init(){
	transferIsInitialized = false;
	programmer_AES_reinitialization_wait_time_s = 0;
	programer_readMetaData();
}

crystalBoolResult_t programmerErase(){
#if 1
	transferIsInitialized = false;
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
	transferIsInitialized = false;
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
		portFlashRead(programmerGetApplicationEntryPoint()+pos, flash_bloc_buffer, size);
		sha256_update(&sha56_ctx,flash_bloc_buffer,size);
		pos += size;
	}

	sha256_final(&sha56_ctx,sha256_calced_hash);

#if CHECKSUM_SIZE != FLASH_BLOCK_BUFFER_SIZE
#error "both sha256 lengths should be the same"
#endif

	if (memcmp(sha256_calced_hash,firmwareMetaData.d.sha256,CHECKSUM_SIZE) == 0){
		firmwareMetaData.d.checksumVerified = 1;

		if (firmwareMetaData.d.usedCrypto == crystalBoolCrypto_Plain){
			firmwareMetaData.d.checksumVerifiedByCrypto = 0;
		}else{
			firmwareMetaData.d.checksumVerifiedByCrypto = 1;
		}
	}else{
		firmwareMetaData.d.checksumVerified = 0;
		firmwareMetaData.d.checksumVerifiedByCrypto = 0;
		result = crystalBool_Fail;
	}

	programer_writeMetaData();
#endif
	return result;
}

crystalBoolResult_t programmerQuickVerify(void){
	transferIsInitialized = false;
	if (!firmware_meta_is_valid){
		return crystalBool_Fail;
	}
	if (firmwareMetaData.d.checksumVerified == 1){
		return crystalBool_OK;
	}else{
		return programmerVerify();
	}
}

crystalBoolResult_t programmerInitFirmwareTransfer(firmware_descriptor_t *firmwareDescriptor, uint8_t sha256[32], uint8_t aes128_iv[16], const crystalBoolCrypto_t crypto){
	transferIsInitialized = false;
	if (firmwareDescriptor->entryPoint < MINIMAL_APPLICATION_ADDRESS){
		return crystalBool_Fail;
	}

	uint32_t flashSize = portFlashGetFlashSize();
	if (firmwareDescriptor->entryPoint+firmwareDescriptor->size > flashSize+FLASH_ADDRESS){
		return crystalBool_Fail;
	}

#if 0
	if (firmwareDescriptor->entryPoint  != APPLICATION_ADDRESS){
		return crystalBool_Fail;
	}
#endif

	if ((BOOTLOADER_ALLOW_PAIN_TEXT_COMMUNICATION==0) && (crypto == crystalBoolCrypto_Plain)){
		return crystalBool_Fail;
	}

	if ((BOOTLOADER_WITH_DECRYPT_SUPPORT==0) && (crypto == crystalBoolCrypto_AES)){
		return crystalBool_Fail;
	}

	if (	(BOOTLOADER_AES_REINITIALISATION_WAITTIME_s) &&
			(firmwareMetaData.d.checksumVerifiedByCrypto ==0) &&
			(crypto == crystalBoolCrypto_AES) &&
			(BOOTLOADER_AES_REINITIALISATION_WAITTIME_s > programmer_AES_reinitialization_wait_time_s)){
		return crystalBool_TryAgainLater;
	}

	memcpy(&firmwareMetaData.d.firmwareDescriptor,firmwareDescriptor,sizeof(firmware_descriptor_t));

	programmWritePointerAddress = programmerGetApplicationEntryPoint();
	programmReadPointerAddress  = programmerGetApplicationEntryPoint();
	cryptoUsed = crypto;
	firmwareMetaData.d.checksumVerified = 0;
	firmwareMetaData.d.checksumVerifiedByCrypto = 0;
	firmwareMetaData.d.usedCrypto = crypto;
	memcpy(firmwareMetaData.d.sha256,sha256,sizeof(firmwareMetaData.d.sha256));
	programer_writeMetaData();

	memcpy(aes128_iv_buf,aes128_iv,AES_128_IV_LENGTH);
	memcpy(aes128_key_local,aes_128_key,AES_128_IV_LENGTH);

	pointerToAESkey = aes128_key_local;
	pointerToAESiv = aes128_iv_buf;

	transferIsInitialized = true;
	programmer_AES_reinitialization_wait_time_s = 0;
	return crystalBool_OK;
}

firmware_descriptor_t programmerGetFirmwareDescriptor( ){
	firmware_descriptor_t result;
	memcpy(&result,&firmwareMetaData.d.firmwareDescriptor,sizeof(result));
	transferIsInitialized = false;
	return result;
}

crystalBoolResult_t programmerWriteBlock(uint8_t *data, size_t size){
#if 1
	//programmer_decode_block(programmWritePointerAddress, data,  &size);
	bool portFlashWrite_result;
	if (transferIsInitialized == false){
		return crystalBool_Fail;
	}

#if BOOTLOADER_WITH_DECRYPT_SUPPORT
	uint8_t data_decrypted[TRANSMISSION_BLOCK_SIZE];
	uint8_t *dataToBeFlashed = data;
	if (cryptoUsed == crystalBoolCrypto_AES){
		dataToBeFlashed = data_decrypted;
		AES128_CBC_decrypt_buffer(data_decrypted, data , size, pointerToAESkey, pointerToAESiv);
		pointerToAESkey = NULL;
		pointerToAESiv = NULL;
	}else{

	}
	portFlashWrite_result = portFlashWrite(programmWritePointerAddress, dataToBeFlashed,  size);
#else
	portFlashWrite_result = portFlashWrite(programmWritePointerAddress, data,  size);
#endif
	if (portFlashWrite_result){
		if (!portFlashVerifyAgainstBuffer(programmWritePointerAddress, dataToBeFlashed,  size)){
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
#if BOOTLOADER_ALLOW_PAIN_TEXT_COMMUNICATION
	if (portFlashRead(programmReadPointerAddress, data,  size)){
		programmReadPointerAddress += size;
		return crystalBool_OK;
	}else{
		return crystalBool_Fail;
	}
#else
	return crystalBool_Fail;
#endif
}

void programmerRunApplication(void){
	transferIsInitialized = false;
#if BOOTLOADER_BOOT_APP_USING_RESET
	portFlashRunApplicationAfterReset();
#else
	portFlashRunApplication();
#endif

}



void programmerGetGUID(uint8_t guid[12]){
	transferIsInitialized = false;
	portFlashGetGUID(guid);
}

mcu_descriptor_t programmerGetMCUDescriptor( ){
	transferIsInitialized = false;
	mcu_descriptor_t result;
	memset(&result,0,sizeof(result));

	programmerGetGUID(result.guid);
	result.devID = 						portFlashGetDeviceID();
	result.revision = 					portFlashGetRevisionID();
	result.flashsize = 					portFlashGetFlashSize();
	result.firmwareEntryPoint = 		0xFFFFFFFF;
	result.minimalFirmwareEntryPoint = 	MINIMAL_APPLICATION_ADDRESS;
	result.availFlashSize = 			FLASH_ADDRESS+result.flashsize - MINIMAL_APPLICATION_ADDRESS;
	result.cryptoRequired = 			BOOTLOADER_ALLOW_PAIN_TEXT_COMMUNICATION==0;
	result.protectionLevel =			portFlashGetProtectionLevel();
	result.firmwareVerified =			firmwareMetaData.d.checksumVerified;
	return result;
}
//(void)programmer_destroyMetaData();
