#include <stdint.h>

#ifndef RPC_TRANMISSION_SERVER_APP_QT2MCU_H
#define RPC_TRANMISSION_SERVER_APP_QT2MCU_H
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#pragma RPC prefix RPC_TRANSMISSION_
#pragma RPC version_number 1
#pragma RPC command_id_start 8


typedef enum {rpcLEDStatus_none,rpcLEDStatus_off,rpcLEDStatus_on} rpcLEDStatus_t;

typedef enum {crystalBool_VerifyError,crystalBool_VerifySuccess} firmwareVerifyResult_t;
typedef enum {crystalBool_Fail,crystalBool_OK} crystalBoolResult_t;

typedef struct{
	uint32_t githash;
	uint32_t gitDate_unix;
	char name[8];
	char version[8];
} firmware_descriptor_t;

typedef struct{
	uint32_t guid;
	uint32_t flashsize;
	uint32_t availFlashSize;
	uint16_t devID;
	uint16_t revision;
} mcu_descriptor_t;

typedef struct{
	uint32_t githash;
	uint32_t gitDate_unix;

	uint32_t serialnumber;
	uint32_t guid;
	uint16_t deviceID;
	uint8_t boardRevision;

	char name[8];
	char version[8];
} device_descriptor_t;

//we want to the testapplication to be able to enumerate devices
#pragma RPC ID mcuReboot 2
device_descriptor_t mcuGetDeviceDescriptor(void);

//we want to be able to send the reboot command to the application from the bootloader pc application
#pragma RPC ID mcuReboot 4
void mcuReboot(void);

//we want to be able to spam the device with mcuEnterProgrammerMode commands while starting to prevent to load application
#pragma RPC ID mcuEnterProgrammerMode 6
void mcuEnterProgrammerMode(void);

crystalBoolResult_t mcuSetApplicationAddress(uint32_t applicationAddress);
uint32_t mcuGetApplicationAddress(void);

void mcuResetReadWritePointerToApplicationAddress(void);
crystalBoolResult_t mcuWriteFirmwareBlock(uint8_t data_in[128]);
crystalBoolResult_t mcuReadFirmwareBlock(uint8_t data_out[128]);
crystalBoolResult_t mcuEraseFlash();

firmwareVerifyResult_t mcuVerifyFirmware();
firmware_descriptor_t mcuGetFirmwareDescriptor( );
mcu_descriptor_t mcuGetMCUDescriptor( );



void mcuRunApplication(void);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
