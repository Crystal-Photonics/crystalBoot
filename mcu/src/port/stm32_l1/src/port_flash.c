/*
 * port_flash.c
 *
 *  Created on: 17.10.2016
 *      Author: ak
 */

#include "port_flash.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include "port_board.h"
#include "main.h"
#include "port_chip.h"
#include "programmer.h"

typedef void (*pFunction)(void);

static pFunction Jump_To_Application;
static uint32_t stackAddressOfApplication;
static uint32_t JumpAddress;

static volatile uint8_t firmwareDescriptorBuffer[FIRMWARE_DESCRIPTION_BUFFER_SIZE] __attribute__((section(".FirmwareDescriptorBuffer")));

#if 0
/**
 * @brief  Convert an Integer to a string
 * @param  str: The string
 * @param  intnum: The intger to be converted
 * @retval None
 */
void Int2Str(uint8_t* str, int32_t intnum)
{
	uint32_t i, Div = 1000000000, j = 0, Status = 0;

	for (i = 0; i < 10; i++)
	{
		str[j++] = (intnum / Div) + 48;

		intnum = intnum % Div;
		Div /= 10;
		if ((str[j-1] == '0') & (Status == 0))
		{
			j = 0;
		}
		else
		{
			Status++;
		}
	}
}

/**
 * @brief  Convert a string to an integer
 * @param  inputstr: The string to be converted
 * @param  intnum: The intger value
 * @retval 1: Correct
 *         0: Error
 */
uint32_t Str2Int(uint8_t *inputstr, int32_t *intnum)
{
	uint32_t i = 0, res = 0;
	uint32_t val = 0;

	if (inputstr[0] == '0' && (inputstr[1] == 'x' || inputstr[1] == 'X'))
	{
		if (inputstr[2] == '\0')
		{
			return 0;
		}
		for (i = 2; i < 11; i++)
		{
			if (inputstr[i] == '\0')
			{
				*intnum = val;
				/* return 1; */
				res = 1;
				break;
			}
			if (ISVALIDHEX(inputstr[i]))
			{
				val = (val << 4) + CONVERTHEX(inputstr[i]);
			}
			else
			{
				/* return 0, Invalid input */
				res = 0;
				break;
			}
		}
		/* over 8 digit hex --invalid */
		if (i >= 11)
		{
			res = 0;
		}
	}
	else /* max 10-digit decimal input */
	{
		for (i = 0;i < 11;i++)
		{
			if (inputstr[i] == '\0')
			{
				*intnum = val;
				/* return 1 */
				res = 1;
				break;
			}
			else if ((inputstr[i] == 'k' || inputstr[i] == 'K') && (i > 0))
			{
				val = val << 10;
				*intnum = val;
				res = 1;
				break;
			}
			else if ((inputstr[i] == 'm' || inputstr[i] == 'M') && (i > 0))
			{
				val = val << 20;
				*intnum = val;
				res = 1;
				break;
			}
			else if (ISVALIDDEC(inputstr[i]))
			{
				val = val * 10 + CONVERTDEC(inputstr[i]);
			}
			else
			{
				/* return 0, Invalid input */
				res = 0;
				break;
			}
		}
		/* Over 10 digit decimal --invalid */
		if (i >= 11)
		{
			res = 0;
		}
	}

	return res;
}

/**
 * @brief  Get an integer from the HyperTerminal
 * @param  num: The inetger
 * @retval 1: Correct
 *         0: Error
 */
uint32_t GetIntegerInput(int32_t * num)
{
	uint8_t inputstr[16];

	while (1)
	{
		GetInputString(inputstr);
		if (inputstr[0] == '\0') continue;
		if ((inputstr[0] == 'a' || inputstr[0] == 'A') && inputstr[1] == '\0')
		{
			SerialPutString("User Cancelled \r\n");
			return 0;
		}

		if (Str2Int(inputstr, num) == 0)
		{
			SerialPutString("Error, Input again: \r\n");
		}
		else
		{
			return 1;
		}
	}
}
#endif
#if 0
/**
 * @brief  Test to see if a key has been pressed on the HyperTerminal
 * @param  key: The key pressed
 * @retval 1: Correct
 *         0: Error
 */
uint32_t SerialKeyPressed(uint8_t *key)
{

	if ( USART_GetFlagStatus(EVAL_COM1, USART_FLAG_RXNE) != RESET)
	{
		*key = (uint8_t)EVAL_COM1->DR;
		return 1;
	}
	else
	{
		return 0;
	}
}

/**
 * @brief  Get a key from the HyperTerminal
 * @param  None
 * @retval The Key Pressed
 */
uint8_t GetKey(void)
{
	uint8_t key = 0;

	/* Waiting for user input */
	while (1)
	{
		if (SerialKeyPressed((uint8_t*)&key)) break;
	}
	return key;

}
#endif

#if 0
/**
 * @brief  Get Input string from the HyperTerminal
 * @param  buffP: The input string
 * @retval None
 */
void GetInputString (uint8_t * buffP)
{
	uint32_t bytes_read = 0;
	uint8_t c = 0;
	do
	{
		c = GetKey();
		if (c == '\r')
			break;
		if (c == '\b') /* Backspace */
		{
			if (bytes_read > 0)
			{
				SerialPutString("\b \b");
				bytes_read --;
			}
			continue;
		}
		if (bytes_read >= CMD_STRING_SIZE )
		{
			SerialPutString("Command string size overflow\r\n");
			bytes_read = 0;
			continue;
		}
		if (c >= 0x20 && c <= 0x7E)
		{
			buffP[bytes_read++] = c;
			SerialPutChar(c);
		}
	}
	while (1);
	SerialPutString(("\n\r"));
	buffP[bytes_read] = '\0';
}
#endif

#if 0
/**
 * @brief  Calculate the number of pages
 * @param  Size: The image size
 * @retval The number of pages
 */
uint32_t FLASH_PagesMask(__IO uint32_t Size)
{
	uint32_t pagenumber = 0x0;
	uint32_t size = Size;

	if ((size % FLASH_PAGE_SIZE) != 0)
	{
		pagenumber = (size / FLASH_PAGE_SIZE) + 1;
	}
	else
	{
		pagenumber = size / FLASH_PAGE_SIZE;
	}
	return pagenumber;
}
#endif

#if 1
// static __RAM_FUNC
static bool portFlashEraseFlash_(uint32_t startAddress, uint32_t size) {
    /*
     * This function falls into a sort of lowpower mode when running under Debugger(segger jlink) as soon as it reaches the second
     * memory bank address > ~0x8040000ul
     *
     */
    bool MemoryProgramOK = true;
    CLEAR_LED_OK();
    FLASH_Status FLASHStatus = FLASH_COMPLETE;
    printf("erase start@ %" PRIx32 ", size = %" PRIx32 "\n", startAddress, size);
    if (size % FLASH_PAGE_SIZE) { // size is not multiple of pagesize/2
        return false;
    }
    if (startAddress % FLASH_PAGE_SIZE) { // address is not multiple of
                                          // pagesize/2
        return false;
    }
    if (startAddress + size > FLASH_END_ADDR) { // buffer exceeds flashsize
        return false;
    }
#if 1
    FLASH_Unlock();
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR | FLASH_FLAG_OPTVERRUSR);

    /* Erase the FLASH Program memory pages */
    const uint32_t numOfPages = size / FLASH_PAGE_SIZE;

    for (uint32_t pageOffset = 0; pageOffset < numOfPages; pageOffset++) {
        const uint32_t flashAddress = startAddress + FLASH_PAGE_SIZE * pageOffset;

        if (pageOffset % 16 == 0) {
            printf("eraseing..%" PRIx32 " of %" PRIx32 "\n", flashAddress, numOfPages);
        }

        FLASHStatus = FLASH_ErasePage(flashAddress);
        if (FLASHStatus != FLASH_COMPLETE) {
            printf("err %d at %" PRIu32 "\n", FLASHStatus, pageOffset);
            if (FLASHStatus == FLASH_BUSY) {
                MemoryProgramOK = false;
            }
            if (FLASHStatus == FLASH_ERROR_WRP) {
                MemoryProgramOK = false;
            }
            if (FLASHStatus == FLASH_ERROR_PROGRAM) {
                MemoryProgramOK = false;
            }
            if (FLASHStatus == FLASH_TIMEOUT) {
                MemoryProgramOK = false;
            }
            MemoryProgramOK = false;
            break;
        } else {
            FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR | FLASH_FLAG_OPTVERRUSR);
        }
    }

    FLASH_Lock();
#endif
    if (MemoryProgramOK) {
        SET_LED_BLUE();
        printf("erase ok\n");
    } else {
        CLEAR_LED_BLUE();
    }
    return MemoryProgramOK;
}

static bool portFlashEraseFlash(uint32_t startAddress, uint32_t size) {
    return portFlashEraseFlash_(startAddress, size);
    // return true;
}
#endif

bool portFlashSaveFirmwareDescriptorBuffer(uint8_t *buffer, const size_t size) {
    bool result;
    if (size > FIRMWARE_DESCRIPTION_BUFFER_SIZE) {
        return false;
    }
    uint8_t buffer256[FIRMWARE_DESCRIPTION_BUFFER_SIZE];
    memset(buffer256, 0, sizeof(buffer256));
    memcpy(buffer256, buffer, size);

    result = portFlashEraseFlash((uint32_t)firmwareDescriptorBuffer, FIRMWARE_DESCRIPTION_BUFFER_SIZE);
    if (!result) {
        return false;
    }
    return portFlashWrite((uint32_t)firmwareDescriptorBuffer, buffer256, FIRMWARE_DESCRIPTION_BUFFER_SIZE);
}

bool portFlashReadFirmwareDescriptorBuffer(uint8_t *buffer, const size_t size) {
    if (size > FIRMWARE_DESCRIPTION_BUFFER_SIZE) {
        return false;
    }
    uint8_t buffer256[FIRMWARE_DESCRIPTION_BUFFER_SIZE];

    bool result = portFlashRead((uint32_t)firmwareDescriptorBuffer, buffer256, FIRMWARE_DESCRIPTION_BUFFER_SIZE);
    if (!result) {
        return false;
    }
    memcpy(buffer, buffer256, size);
    // memcpy(buffer,&firmwareDescriptorBuffer,size);
    return true;
}

static void uint32_tToBuffer(uint8_t *out_buffer, const uint32_t in) {
    out_buffer[0] = (in)&0xFF;
    out_buffer[1] = (in >> 8) & 0xFF;
    out_buffer[2] = (in >> 16) & 0xFF;
    out_buffer[3] = (in >> 24) & 0xFF;
}

static uint32_t bufferToUint32_t(uint8_t *buffer) {
    uint32_t result = 0;
    result = buffer[0];
    result |= buffer[1] << 8;
    result |= buffer[2] << 16;
    result |= buffer[3] << 24;

    return result;
}

bool portFlashWrite(const uint32_t startAddress, uint8_t *buffer, const size_t size) {
    FLASH_Status FLASHStatus = FLASH_COMPLETE;
    bool MemoryProgramOK = true;
    const uint32_t HALF_PAGE_SIZE = FLASH_PAGE_SIZE / 2;

    CLEAR_LED_OK();

    if (size % HALF_PAGE_SIZE) { // size is not multiple of pagesize/2
        return false;
    }
    if (startAddress % HALF_PAGE_SIZE) { // address is not multiple of pagesize/2
        return false;
    }
    if (startAddress + size > FLASH_END_ADDR) { // buffer exceeds flashsize
        return false;
    }

    FLASH_Unlock();

    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR | FLASH_FLAG_OPTVERRUSR);

    const uint32_t numOfHalfPages = size / HALF_PAGE_SIZE;

    for (uint32_t pageOffset = 0; pageOffset < numOfHalfPages; pageOffset++) {
        const uint32_t HALF_PAGE_WORD_COUNT = HALF_PAGE_SIZE / 4;

        uint32_t halfPageBuffer[HALF_PAGE_WORD_COUNT];

        for (uint32_t i = 0; i < HALF_PAGE_WORD_COUNT; i++) {
            halfPageBuffer[i] = bufferToUint32_t(&buffer[i * 4]);
        }

        const uint32_t flashAddress = startAddress + HALF_PAGE_SIZE * pageOffset;

        __disable_irq(); // FLASH_ProgramHalfPage is a ramfunction and must not be
                         // disturbed by interrupts
                         // since the vector table is not initialized in ram?
        FLASHStatus = FLASH_ProgramHalfPage(flashAddress, halfPageBuffer);
        __enable_irq();

        if (FLASHStatus != FLASH_COMPLETE) {
            // printf("err write %d at %"PRIu32"\n",FLASHStatus,Address);
            if (FLASHStatus == FLASH_BUSY) {
                MemoryProgramOK = false;
            }
            if (FLASHStatus == FLASH_ERROR_WRP) {
                MemoryProgramOK = false;
            }
            if (FLASHStatus == FLASH_ERROR_PROGRAM) {
                MemoryProgramOK = false;
            }
            if (FLASHStatus == FLASH_TIMEOUT) {
                MemoryProgramOK = false;
            }
            MemoryProgramOK = false;
            FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR | FLASH_FLAG_OPTVERRUSR);
            break;
        }
    }
    FLASH_Lock();
    if (MemoryProgramOK) {
        SET_LED_OK();
        // printf("write ok\n");
    } else {
        CLEAR_LED_BLUE();
    }
    return MemoryProgramOK;
}

bool portFlashEraseApplication() {
    return portFlashEraseFlash(MINIMAL_APPLICATION_ADDRESS, FLASH_END_ADDR - MINIMAL_APPLICATION_ADDRESS);
}

bool portFlashEraseEEPROM() {
    FLASH_Status result = FLASH_BUSY;
    DATA_EEPROM_Unlock();
    for (uint32_t address = DATA_EEPROM_START_ADDR; address < DATA_EEPROM_END_ADDR; address += 4) {
        result = DATA_EEPROM_EraseWord(address);
        if (result != FLASH_COMPLETE) {
            break;
        }
    }
    DATA_EEPROM_Lock();
    return result == FLASH_COMPLETE;
}

bool portFlashVerifyAgainstBuffer(const uint32_t startAddress, uint8_t *buffer, const size_t size) {
    uint32_t flashAddress = startAddress;
    CLEAR_LED_BLUE();
    /* Check the written data */
    for (uint32_t i = 0; i < size / 4; i++) {
        uint8_t inBuf[4];
        uint32_t inWord = *(__IO uint32_t *)flashAddress;

        uint32_tToBuffer(inBuf, inWord);

        if (memcmp(&buffer[i * 4], inBuf, 4)) {
            return false;
        }
        flashAddress += 4;
    }
    SET_LED_BLUE();
    return true;
}

bool portFlashRead(const uint32_t startAddress, uint8_t *buffer, const size_t size) {
    uint32_t flashAddress = startAddress;
    CLEAR_LED_BLUE();
    if (size % 4) { // size is well aligned for uint32_t?
        return false;
    }
    if (startAddress % 4) { // address is well aligned for uint32_t?
        return false;
    }
    if (startAddress + size > FLASH_END_ADDR) { // buffer exceeds flashsize
        return false;
    }

    /* Check the written data */
    for (uint32_t i = 0; i < size / 4; i++) {
        uint32_t inWord = *(__IO uint32_t *)flashAddress;

        uint32_tToBuffer(&buffer[i * 4], inWord);

        flashAddress += 4;
    }
    SET_LED_BLUE();
    return true;
}

void portFlashRunApplicationAfterReset() {
    printf("starting app..\n");
    for (int i = 0; i < 10000; i++) {
        __asm__("NOP");
    }
    port_programDirectApplicationLaunch();
    NVIC_SystemReset();
}

void portFlashRunApplication() {
    //#define ApplicationAddress    0x8003000
    // MINIMAL_APPLICATION_ADDRESS
    /* Image:
     * 4 bytes:	pointer to stack location. In our case something like  0x20 01 3F FF
     * 4 bytes:	reset vector
     *
     *
     * Test if user code is programmed starting from address "ApplicationAddress"
     * magic number 0x2FFE0000 results from ram size aka stack pointer
     */
    stackAddressOfApplication = *(__IO uint32_t *)programmerGetApplicationEntryPoint();
    if ((stackAddressOfApplication & 0x2FFE0000) == 0x20000000) {
        static uint32_t applicationResetVector = 0;
        applicationResetVector = programmerGetApplicationEntryPoint() + 4;
        JumpAddress = *(__IO uint32_t *)(applicationResetVector);

        __disable_irq();
        Jump_To_Application = (pFunction)JumpAddress;
        /* Initialize user application's Stack Pointer
         *
         * !!!!Often the application initializes the isr Vector table in startup
         *code to wrong address.
         * check system_stm32l1xx.c for this.
         *
         * can be fixed with
         *	  extern uint32_t _start_isr_vector;
         *
         *    volatile uint32_t ISR_VECTOR_TABLE_OFFSET =
         *(uint32_t)&_start_isr_vector;
         *
         * 	  ISR_VECTOR_TABLE_OFFSET = ISR_VECTOR_TABLE_OFFSET - FLASH_BASE;
         *
         * 	  SCB->VTOR = FLASH_BASE | ISR_VECTOR_TABLE_OFFSET;
         *
         *
         * 	  and:
         *
         * 	    .isr_vector :
         *		  {
         *			. = ALIGN(4);
         *			PROVIDE(_start_isr_vector = .);
         *			KEEP(*(.isr_vector))
         *			. = ALIGN(4);
         *		  } >FLASH
         *
         * in linker script
         *
         * */
        port_chipDeinit();

        uint32_t vector_table_offset = programmerGetApplicationEntryPoint() - 0x8000000ul;
        NVIC_SetVectorTable(NVIC_VectTab_FLASH, vector_table_offset);

        // for this application it works. but it might be better to do an reset and
        // start the application directly after this
        // before initializing any peripheral

        __set_MSP(stackAddressOfApplication);
        Jump_To_Application();

        // should not reach this
        while (1) {
        }
    }
}

void portFlashGetGUID(uint8_t guid[12]) {
    uint32_t *uid1 = (uint32_t *)UID1;
    uint32_t *uid2 = (uint32_t *)UID2;
    uint32_t *uid3 = (uint32_t *)UID3;

    uint32_tToBuffer(&guid[0], *uid1);
    uint32_tToBuffer(&guid[4], *uid2);
    uint32_tToBuffer(&guid[8], *uid3);
}

uint16_t portFlashGetDeviceID() {
    return DBGMCU_GetDEVID();
}

uint16_t portFlashGetRevisionID() {
    return DBGMCU_GetREVID();
}

uint32_t portFlashGetFlashSize() {
    uint16_t *fsize = (uint16_t *)F_SIZE_ADDRESS;
    uint16_t devID = portFlashGetDeviceID();
    uint32_t result = *fsize & 0xFFFF;

    if (devID == 0x436) {
        if (result & 0xFF) {
            result = 384;
        } else {
            result = 256;
        }
    }
    result *= 1024;
    return result;
}

int portFlashGetProtectionLevel() {
    if (FLASH_OB_GetRDP() == SET) {
        return 1;
    } else {
        return 0;
    }
}

void portFlashSetProtectionLevel(int protectionLevel) {
    if (portFlashGetProtectionLevel() != protectionLevel) {
        FLASH_OB_Unlock();
        // printf("FLASH_OB_unlocked");
        if (protectionLevel == 1) {
            FLASH_OB_RDPConfig(OB_RDP_Level_1);
        } else if (protectionLevel == 0) {
            FLASH_OB_RDPConfig(OB_RDP_Level_0);
        }
        // printf("FLASH_OB_Launch");
        FLASH_OB_Launch();
        // printf("FLASH_OB_LOCK");
        FLASH_OB_Lock();
        // printf("FLASH_OB_Rdy");
    }
}
