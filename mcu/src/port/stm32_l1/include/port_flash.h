/*
 * port_flash.h
 *
 *  Created on: 17.10.2016
 *      Author: ak
 */

#ifndef MCU_SRC_PORT_STM32_L1_INCLUDE_PORT_FLASH_H_
#define MCU_SRC_PORT_STM32_L1_INCLUDE_PORT_FLASH_H_


/* Includes ------------------------------------------------------------------*/
#include "stdio.h"
#include "string.h"
#include "board.h"

/* Exported types ------------------------------------------------------------*/
typedef  void (*pFunction)(void);

/* Exported constants --------------------------------------------------------*/
/* Constants used by Serial Command Line Mode */
#define CMD_STRING_SIZE       128

#define MINIMAL_APPLICATION_ADDRESS  0x8006000


#if 1
#if defined (STM32L1XX_XL)
	#define FLASH_PAGE_SIZE             (256)
	 //#define FLASH_SIZE
	#define FLASH_SIZE                	0x80000
	#define FLASH_END_ADDR				0x08000000+FLASH_SIZE

#else
 #error "Please select first the STM32 device to be used (in stm32f10x.h)"
#endif
#endif
#if 0

/* Compute the FLASH upload image size */
#define FLASH_IMAGE_SIZE                   (uint32_t) (FLASH_SIZE - (ApplicationAddress - 0x08000000))

/* Exported macro ------------------------------------------------------------*/
/* Common routines */

#define IS_AF(c)  ((c >= 'A') && (c <= 'F'))
#define IS_af(c)  ((c >= 'a') && (c <= 'f'))
#define IS_09(c)  ((c >= '0') && (c <= '9'))
#define ISVALIDHEX(c)  IS_AF(c) || IS_af(c) || IS_09(c)
#define ISVALIDDEC(c)  IS_09(c)
#define CONVERTDEC(c)  (c - '0')

#define CONVERTHEX_alpha(c)  (IS_AF(c) ? (c - 'A'+10) : (c - 'a'+10))
#define CONVERTHEX(c)   (IS_09(c) ? (c - '0') : CONVERTHEX_alpha(c))
#endif

#if 0
/* Exported functions ------------------------------------------------------- */
void Int2Str(uint8_t* str,int32_t intnum);
uint32_t Str2Int(uint8_t *inputstr,int32_t *intnum);
uint32_t GetIntegerInput(int32_t * num);
uint32_t SerialKeyPressed(uint8_t *key);
uint8_t GetKey(void);
#endif

void GetInputString(uint8_t * buffP);
//uint32_t FLASH_PagesMask(__IO uint32_t Size);
void FLASH_DisableWriteProtectionPages(void);


bool portFlashEraseApplication();
bool portFlashWrite(const uint32_t startAddress, uint8_t *buffer, const size_t size);
bool portFlashVerifyAgainstBuffer(const uint32_t startAddress, uint8_t *buffer, const size_t size);
bool portFlashRead(const uint32_t startAddress, uint8_t *buffer, const size_t size);

void portFlashVerify();


void portFlashSetProtection();
void portFlashGetProtection();

void portFlashJumpToApplication();

void portFlashGetGUID();
void portFlashGetFlashSize();//F_SIZE
void portFlashGetDeviceID(); //DBGMCU_IDCODE
void portFlashGetRevisionID(); //DBGMCU_IDCODE

#if 0
void Main_Menu(void);
void SerialDownload(void);
void SerialUpload(void);
#endif


#endif /* MCU_SRC_PORT_STM32_L1_INCLUDE_PORT_FLASH_H_ */
