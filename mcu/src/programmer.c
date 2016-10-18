/*
 * programmer.c
 *
 *  Created on: 18.10.2016
 *      Author: ak
 */
#include <string.h>
#include "programmer.h"

#define AES_BLOCK_LENGTH 32

static uint8_t progBuffer[AES_BLOCK_LENGTH];

void programmerReset(void){
	memset(progBuffer,AES_BLOCK_LENGTH,0);
}

void programmerWriteBlock(uint8_t *data, size_t size){
	memcpy(progBuffer,data,AES_BLOCK_LENGTH);
}

void programmerReadBlock(uint8_t *data, size_t size){
	memcpy(data,progBuffer,AES_BLOCK_LENGTH);
}
