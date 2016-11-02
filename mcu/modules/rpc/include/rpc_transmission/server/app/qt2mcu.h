#include <stdint.h>

#ifndef RPC_TRANMISSION_SERVER_APP_QT2MCU_H
#define RPC_TRANMISSION_SERVER_APP_QT2MCU_H
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#pragma RPC prefix RPC_TRANSMISSION_
#pragma RPC version_number 1
#pragma RPC command_id_start 8

#include "../../../../../_deviceIDs/include/deviceIDs.h"

typedef enum {rpcLEDStatus_none,rpcLEDStatus_off,rpcLEDStatus_on} rpcLEDStatus_t;

typedef enum {crystalBool_Fail,crystalBool_OK} crystalBoolResult_t;
typedef enum {crystalBoolCrypto_Plain,crystalBoolCrypto_AES} crystalBoolCrypto_t;

typedef struct{
	uint32_t githash;
	uint32_t gitDate_unix;
	uint16_t nameCRC16;
	uint32_t size;
	uint32_t entryPoint;
	char name[12];
	char version[8];
} firmware_descriptor_t;


typedef struct{
	uint8_t guid[12];
	uint32_t flashsize;
	uint32_t availFlashSize;
	uint16_t devID;
	uint16_t revision;
	uint32_t firmwareEntryPoint;
	uint32_t minimalFirmwareEntryPoint;
	uint8_t cryptoRequired;
} mcu_descriptor_t;


//we want to the testapplication to be able to enumerate devices
#pragma RPC ID mcuReboot 2
device_descriptor_v1_t mcuGetDeviceDescriptor(void);

//we want to be able to send the reboot command to the application from the bootloader pc application
#pragma RPC ID mcuReboot 4
void mcuReboot(void);

//we want to be able to spam the device with mcuEnterProgrammerMode commands while starting to prevent to load application
#pragma RPC ID mcuEnterProgrammerMode 6
void mcuEnterProgrammerMode(void);

crystalBoolResult_t mcuInitFirmwareTransfer( firmware_descriptor_t firmwareDescriptor_in[1], uint8_t sha256_in[32], crystalBoolCrypto_t crypto);
firmware_descriptor_t mcuGetFirmwareDescriptor( );

crystalBoolResult_t mcuWriteFirmwareBlock(uint8_t data_in[128]);
crystalBoolResult_t mcuReadFirmwareBlock(uint8_t data_out[128]);
crystalBoolResult_t mcuEraseFlash();

crystalBoolResult_t mcuVerifyFirmware();
mcu_descriptor_t mcuGetMCUDescriptor( );





void mcuRunApplication(void);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
