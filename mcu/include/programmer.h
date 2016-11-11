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

#define CHECKSUM_SIZE 32
typedef struct{
	firmware_descriptor_t firmwareDescriptor;

	bool checksumVerified;

	uint8_t sha256[CHECKSUM_SIZE];

}firmware_meta_data_t;

typedef struct{
	uint16_t crc16OfMetaData;
	firmware_meta_data_t d;
}firmware_meta_t;



void programmer_init();
crystalBoolResult_t programmerErase(void);
crystalBoolResult_t programmerVerify(void);
crystalBoolResult_t programmerQuickVerify(void);
crystalBoolResult_t programmerInitFirmwareTransfer(firmware_descriptor_t *firmwareDescriptor,  uint8_t sha256[32], uint8_t aes128_iv[16], const crystalBoolCrypto_t crypto );
crystalBoolResult_t programmerWriteBlock(uint8_t *data, size_t size);
crystalBoolResult_t programmerReadBlock(uint8_t *data, size_t size);

uint32_t programmerGetApplicationEntryPoint();

void programmerRunApplication(void);
void programmerIncrementAESReInitWaitTime_s(void);


firmware_descriptor_t programmerGetFirmwareDescriptor( );
mcu_descriptor_t programmerGetMCUDescriptor( );
void programmerGetGUID(uint8_t guid[12]);

#endif /* MCU_INCLUDE_PROGRAMMER_H_ */
