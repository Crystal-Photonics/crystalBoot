/*
 * programmer.c
 *
 *  Created on: 18.10.2016
 *      Author: ak
 */
#include <string.h>
#include "programmer.h"
#include "port_flash.h"

#define BLOCK_LENGTH 128
static uint32_t programmWritePointerAddress = MINIMAL_APPLICATION_ADDRESS;
static uint32_t programmReadPointerAddress = MINIMAL_APPLICATION_ADDRESS;

static uint8_t progBuffer[BLOCK_LENGTH];

crystalBoolResult_t programmerErase(){
	if (portFlashEraseApplication()){
		return crystalBool_OK;
	}else{
		return crystalBool_Fail;
	}
}

void programmerResetReadWritePointerToApplicationAddress(void){
	memset(progBuffer,BLOCK_LENGTH,0);
	programmWritePointerAddress = MINIMAL_APPLICATION_ADDRESS;
	programmReadPointerAddress  = MINIMAL_APPLICATION_ADDRESS;
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
