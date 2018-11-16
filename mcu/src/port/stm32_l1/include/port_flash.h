/*
 * port_flash.h
 *
 *  Created on: 17.10.2016
 *      Author: ak
 */

#ifndef MCU_SRC_PORT_STM32_L1_INCLUDE_PORT_FLASH_H_
#define MCU_SRC_PORT_STM32_L1_INCLUDE_PORT_FLASH_H_

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>
#include <string.h> //for size_t
#include "stm32l1xx_conf.h"
#include "stm32l1xx.h"

#define MINIMAL_APPLICATION_ADDRESS 0x8005000
//#define APPLICATION_ADDRESS  				0x8006000
#define FLASH_ADDRESS FLASH_BASE
#define FIRMWARE_DESCRIPTION_BUFFER_SIZE FLASH_PAGE_SIZE

#if 1
#if defined(STM32L1XX_XL)
#define FLASH_PAGE_SIZE (256)
//#define FLASH_SIZE
#define FLASH_SIZE 0x80000
#define FLASH_END_ADDR 0x08000000 + FLASH_SIZE
#define F_SIZE_ADDRESS ((uint32_t)0x1FF800CC) /* on 0x427 Medium+ and 0x436 HD devices */

#define DATA_EEPROM_START_ADDR 0x08080000
#define DATA_EEPROM_END_ADDR 0x08083FFF
#else
#error "Please select first the STM32 device to be used"
#endif
#endif
#if 0


/* Exported macro ------------------------------------------------------------*/
/* Common routines */

#define IS_AF(c) ((c >= 'A') && (c <= 'F'))
#define IS_af(c) ((c >= 'a') && (c <= 'f'))
#define IS_09(c) ((c >= '0') && (c <= '9'))
#define ISVALIDHEX(c) IS_AF(c) || IS_af(c) || IS_09(c)
#define ISVALIDDEC(c) IS_09(c)
#define CONVERTDEC(c) (c - '0')

#define CONVERTHEX_alpha(c) (IS_AF(c) ? (c - 'A' + 10) : (c - 'a' + 10))
#define CONVERTHEX(c) (IS_09(c) ? (c - '0') : CONVERTHEX_alpha(c))
#endif

#if 0
/* Exported functions ------------------------------------------------------- */
void Int2Str(uint8_t* str,int32_t intnum);
uint32_t Str2Int(uint8_t *inputstr,int32_t *intnum);
uint32_t GetIntegerInput(int32_t * num);
uint32_t SerialKeyPressed(uint8_t *key);
uint8_t GetKey(void);
#endif

void GetInputString(uint8_t *buffP);
// uint32_t FLASH_PagesMask(__IO uint32_t Size);
void FLASH_DisableWriteProtectionPages(void);

bool portFlashEraseApplication();
bool portEEPROMErase();

uint32_t portEEPROMGetStartIndex();
uint32_t portEEPROMGetSize();
uint32_t portEEPROMGetEndAddress();

bool portEEPROMRead(uint16_t address, uint8_t *buffer, uint16_t size);
bool portEEPROMWrite(uint16_t address, uint8_t *buffer, uint16_t size);

bool portFlashSaveFirmwareDescriptorBuffer(uint8_t *buffer, const size_t size);
bool portFlashReadFirmwareDescriptorBuffer(uint8_t *buffer, const size_t size);
bool portFlashWrite(const uint32_t startAddress, uint8_t *buffer, const uint32_t size);
bool portFlashVerifyAgainstBuffer(const uint32_t startAddress, uint8_t *buffer, const size_t size);
bool portFlashRead(const uint32_t startAddress, uint8_t *buffer, const size_t size);

void portFlashRunApplication();

void portFlashVerify();

void portFlashJumpToApplication();
void portFlashRunApplicationAfterReset();

void portFlashGetGUID(uint8_t guid[12]);
uint32_t portFlashGetFlashSize();  // F_SIZE
uint16_t portFlashGetDeviceID();   // DBGMCU_IDCODE
uint16_t portFlashGetRevisionID(); // DBGMCU_IDCODE

int portFlashGetProtectionLevel();
void portFlashSetProtectionLevel(int protectionLevel);

#if 0
void Main_Menu(void);
void SerialDownload(void);
void SerialUpload(void);
#endif

#endif /* MCU_SRC_PORT_STM32_L1_INCLUDE_PORT_FLASH_H_ */
