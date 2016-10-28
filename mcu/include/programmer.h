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

typedef struct{
	firmware_descriptor_t firmwareDescriptor;

	bool checksumVerified;
	uint8_t sha256[32];
}firmware_meta_t;

void programmer_init();
crystalBoolResult_t programmerErase(void);
crystalBoolResult_t programmerInitFirmwareTransfer(firmware_descriptor_t *firmwareDescriptor);
crystalBoolResult_t programmerWriteBlock(uint8_t *data, size_t size);
crystalBoolResult_t programmerReadBlock(uint8_t *data, size_t size);

void programmerRunApplication(void);


firmware_descriptor_t programmerGetFirmwareDescriptor( );
mcu_descriptor_t programmerGetMCUDescriptor( );
void programmerGetGUID(uint8_t guid[12]);

#endif /* MCU_INCLUDE_PROGRAMMER_H_ */
