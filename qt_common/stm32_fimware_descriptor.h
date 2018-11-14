#ifndef STM32_FIMWARE_DESCRIPTOR_H
#define STM32_FIMWARE_DESCRIPTOR_H

#include <stdint.h>

#define FLASH_PAGE_SIZE ((uint32_t)256U) /*!< FLASH Page Size in bytes */
#define FIRMWARE_DESCRIPTION_BUFFER_SIZE FLASH_PAGE_SIZE

typedef enum { crystalBoolCrypto_Plain, crystalBoolCrypto_AES } crystalBoolCrypto_t;

typedef struct {
    uint32_t githash;
    uint32_t gitDate_unix;
    uint16_t nameCRC16;
    uint32_t size;
    uint32_t entryPoint;
    char name[12];
    char version[8];
} stm32_firmware_descriptor_t;

#define CHECKSUM_SIZE 32
typedef struct {
    stm32_firmware_descriptor_t firmwareDescriptor;
    uint8_t checksumVerified;
    uint8_t checksumVerifiedByCrypto;
    uint8_t usedCrypto;
    uint8_t sha256[CHECKSUM_SIZE];
} stm32_firmware_meta_data_t;

typedef struct {
    uint16_t crc16OfMetaData;
    stm32_firmware_meta_data_t d;
} stm32_firmware_meta_t;

#endif // STM32_FIMWARE_DESCRIPTOR_H
