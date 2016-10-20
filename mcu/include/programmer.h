/*
 * programmer.h
 *
 *  Created on: 18.10.2016
 *      Author: ak
 */

#ifndef MCU_INCLUDE_PROGRAMMER_H_
#define MCU_INCLUDE_PROGRAMMER_H_
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "rpc_transmission/server/app/qt2mcu.h"

crystalBoolResult_t programmerErase(void);
void programmerResetReadWritePointerToApplicationAddress(void);
crystalBoolResult_t programmerWriteBlock(uint8_t *data, size_t size);
crystalBoolResult_t programmerReadBlock(uint8_t *data, size_t size);



#endif /* MCU_INCLUDE_PROGRAMMER_H_ */
