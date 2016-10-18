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

static uint8_t progBuffer[BLOCK_LENGTH];

bool programmerErase(){
	return portFlashEraseApplication();
}

void programmerReset(void){
	memset(progBuffer,BLOCK_LENGTH,0);
}

void programmerWriteBlock(uint8_t *data, size_t size){
	portFlashWrite(data,  size);
	//memcpy(progBuffer,data,BLOCK_LENGTH);
	//progBuffer[0]+=1;
}

void programmerReadBlock(uint8_t *data, size_t size){
	memcpy(data,progBuffer,BLOCK_LENGTH);
}
