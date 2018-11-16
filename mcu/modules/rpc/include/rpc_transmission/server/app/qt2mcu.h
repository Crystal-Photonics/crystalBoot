#include <stdint.h>

#ifndef RPC_TRANMISSION_SERVER_APP_QT2MCU_H
#define RPC_TRANMISSION_SERVER_APP_QT2MCU_H
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#pragma RPC prefix RPC_TRANSMISSION_
#pragma RPC version_number 1
#pragma RPC command_id_start 8

#define TRANSMISSION_BLOCK_SIZE 128

#include "..\..\..\..\..\..\bootloaderconfig\board\bootloader_config.h"

typedef enum { rpcLEDStatus_none, rpcLEDStatus_off, rpcLEDStatus_on } rpcLEDStatus_t;

typedef enum { crystalBool_Fail, crystalBool_OK, crystalBool_TryAgainLater } crystalBoolResult_t;
typedef enum { crystalBoolCrypto_Plain, crystalBoolCrypto_AES } crystalBoolCrypto_t;

typedef struct {
    uint32_t githash;
    uint32_t gitDate_unix;
    uint16_t nameCRC16;
    uint32_t size;
    uint32_t entryPoint;

    char name[12];
    char version[8];
} firmware_descriptor_t;

typedef struct {
    uint8_t guid[12];
    uint32_t flashsize;
    uint32_t availFlashSize;
    uint16_t devID;
    uint16_t revision;
    uint32_t firmwareEntryPoint;
    uint32_t minimalFirmwareEntryPoint;
    uint8_t cryptoRequired;
    uint8_t protectionLevel;
    uint8_t firmwareVerified;
} mcu_descriptor_t;

// we want to the testapplication to be able to enumerate devices
#pragma RPC ID mcuReboot 2
device_descriptor_v1_t mcuGetDeviceDescriptor(void);

// we want to be able to spam the device with mcuEnterProgrammerMode commands while starting to prevent to load application
#pragma RPC ID mcuEnterProgrammerMode 6
void mcuEnterProgrammerMode(void);

crystalBoolResult_t mcuInitFirmwareTransfer(firmware_descriptor_t firmwareDescriptor_in[1], uint8_t sha256_in[32], uint8_t aes128_iv_in[16],
                                            crystalBoolCrypto_t crypto);
firmware_descriptor_t mcuGetFirmwareDescriptor();

// with length = TRANSMISSION_BLOCK_SIZE

crystalBoolResult_t mcuWriteFirmwareBlock(uint8_t data_in[128]);
crystalBoolResult_t mcuReadFirmwareBlock(uint8_t data_out[128]);
crystalBoolResult_t mcuEraseFlash();

crystalBoolResult_t mcuEEPROMInitTransfer();
crystalBoolResult_t mcuEEPROMGetSize(uint16_t size_bytes_out[1]);
crystalBoolResult_t mcuEEPROMRead(uint8_t data_out[128], uint8_t size);
crystalBoolResult_t mcuEEPROMWrite(uint8_t data_in[128], uint8_t size);
crystalBoolResult_t mcuEEPROMVerify(uint16_t crc16);
crystalBoolResult_t mcuEEPROMErase();

crystalBoolResult_t mcuVerifyFirmware();
mcu_descriptor_t mcuGetMCUDescriptor();

void mcuRunApplication(void);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
