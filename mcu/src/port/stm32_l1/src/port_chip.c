
#include "port_chip.h"
#include "port_board.h"
#include "port_flash.h"
#include "programmer.h"
#include <stdio.h>

extern uint32_t __firmware_descriptor_buffer_end;

const uint32_t MAGIC_KEY_IN_BACKUP_TO_START_DIRECTLY_IN_APPMODE = 0x5868FE4A;
const uint32_t BACKUPADDRESS_OF_DIRECTLY_START_IN_APPMODE_VALUE = RTC_BKP_DR31;

uint32_t magic_key_which_survives_soft_reset_and_mustn_be_inited __attribute__((section (".un_init")));

#if 0
#define portDISABLE_INTERRUPTS()				ulPortSetInterruptMask()
#define portENABLE_INTERRUPTS()					vPortClearInterruptMask(0)


__attribute__(( naked )) uint32_t ulPortSetInterruptMask( void )
{
	__asm volatile														\
	(																	\
		"	mrs r0, basepri											\n" \
		"	mov r1, %0												\n"	\
		"	msr basepri, r1											\n" \
		"	bx lr													\n" \
		:: "i" ( configMAX_SYSCALL_INTERRUPT_PRIORITY ) : "r0", "r1"	\
	);

	/* This return will not be reached but is necessary to prevent compiler
	warnings. */
	return 0;
}
/*-----------------------------------------------------------*/

__attribute__(( naked )) void vPortClearInterruptMask( uint32_t ulNewMaskValue )
{
	__asm volatile													\
	(																\
		"	msr basepri, r0										\n"	\
		"	bx lr												\n" \
		:::"r0"														\
	);

	/* Just to avoid compiler warnings. */
	( void ) ulNewMaskValue;
}
#endif

void port_EnterCritical( void )
{
#if 0
	portDISABLE_INTERRUPTS();

	__asm volatile( "dsb" );
	__asm volatile( "isb" );
#else
	  __disable_irq();
#endif
}

void port_ExitCritical( void )
{
	#if 0
	portENABLE_INTERRUPTS();
#else
	__enable_irq();
#endif
}

static void port_sysTickConfig(void)
{
	/* Setup SysTick Timer for 1ms interrupts  */
	if (SysTick_Config(SystemCoreClock / 1000))
	{
		/* Capture error */
		while (1);
	}
	/* Configure the SysTick handler priority */
	NVIC_SetPriority(SysTick_IRQn, 0x0);
}

void port_chipInit(){


	SystemCoreClockUpdate();

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

	port_sysTickConfig();

	boardInit();
}


void port_chipDeinit(){

	boardDeinit();
    USART_DeInit(COM_USART_BASE);
	RCC_DeInit();
#if 1
	    SysTick->CTRL = 0;
	    SysTick->LOAD = 0;
	    SysTick->VAL = 0;
#endif
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,DISABLE);
	RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);

}
#if 0
static uint32_t mainBKUPGetSetting(const uint32_t bkp_register, const uint32_t mask, const uint8_t pos){

	uint32_t result = RTC_ReadBackupRegister(bkp_register);
	result &= mask;
	result >>= pos;
	//printf("read backup reg: %"PRIu32", mask: 0x%"PRIX32", pos: %d, result value: %"PRIu32"\n",bkp_register, mask, pos, result);
	return result;
}

static void mainBKUPSetSetting(uint32_t value, const uint32_t bkp_register, const uint32_t mask, const uint8_t pos){
	uint32_t tmp = RTC_ReadBackupRegister(bkp_register);
	value <<= pos;
	if (value & ~mask){
	//	GEN_ASSERT(0,errLog_E_main_backupregister_attempted_write_of_an_illegal_value,"backupregister attempted write of an illegal value\n");
	}
	tmp &= ~mask;
	tmp |= value;
//	printf("write backup reg: %"PRIu32", mask: 0x%"PRIX32", pos: %d, origvalue: %"PRIu32", value written: %"PRIu32"\n",bkp_register, mask, pos, value, tmp);
	RTC_WriteBackupRegister(bkp_register, tmp);
}
#endif

bool port_isDirectApplicationLaunchProgrammed(){
#if 0
	uint32_t val = mainBKUPGetSetting(BACKUPADDRESS_OF_DIRECTLY_START_IN_APPMODE_VALUE,0xFFFFFFFF,0);
	mainBKUPSetSetting(0,BACKUPADDRESS_OF_DIRECTLY_START_IN_APPMODE_VALUE,0xFFFFFFFF,0);
	return val == MAGIC_KEY_IN_BACKUP_TO_START_DIRECTLY_IN_APPMODE;
#else
	uint32_t val = magic_key_which_survives_soft_reset_and_mustn_be_inited;
	magic_key_which_survives_soft_reset_and_mustn_be_inited = 0;
	return val == MAGIC_KEY_IN_BACKUP_TO_START_DIRECTLY_IN_APPMODE;
#endif

}

void port_programDirectApplicationLaunch(void){
	magic_key_which_survives_soft_reset_and_mustn_be_inited = MAGIC_KEY_IN_BACKUP_TO_START_DIRECTLY_IN_APPMODE;
	/*
	mainBKUPSetSetting(MAGIC_KEY_IN_BACKUP_TO_START_DIRECTLY_IN_APPMODE,
			BACKUPADDRESS_OF_DIRECTLY_START_IN_APPMODE_VALUE,
			0xFFFFFFFF,
			0);*/
}



resetReason_t portTestResetSource(void){
	// test the reset flags in order because the pin reset is always set.
	//http://www.micromouseonline.com/2012/03/29/stm32-reset-source/
	resetReason_t result = rer_none;
#if 1
	uint32_t rcc_csr = RCC->CSR;
	uint32_t pwr_csr = PWR->CSR;
	//printf("reset reason PWR_CSR 0x%08"PRIX32 " RCC_CSR 0x%08"PRIX32 " RTC_ISR 0x%08"PRIX32"\n",rcc_csr,pwr_csr,RTC->ISR);

	if ((pwr_csr & PWR_CSR_WUF) && (pwr_csr & PWR_CSR_SBF)){
		//PWR_CSR_WUF: Wakeup flag
		//	This bit is set by hardware and cleared by a system reset or by setting the CWUF bit in the
		//	PWR power control register (PWR_CR)
		//	0: No wakeup event occurred
		//	1: A wakeup event was received from the WKUP pin or from the RTC alarm (Alarm A or
		//	Alarm B), RTC Tamper event, RTC TimeStamp event or RTC Wakeup).

		//PWR_CSR_SBF: Standby flag
		//	This bit is set by hardware and cleared only by a POR/PDR (power on reset/power down reset)
		//	or by setting the CSBF bit in the PWR power control register (PWR_CR)
		//	0: Device has not been in Standby mode
		//	1: Device has been in Standby mode


		  if(RTC->ISR & RTC_ISR_WUTF){
			  result = rer_rtc; 			//works
		  }else{
			  result = rer_wupin;
		  }

	}else if  ( rcc_csr & RCC_CSR_LPWRRSTF){

		//Low-power management reset
		//	There are two ways to generate a low-power management reset:
		//
		//	1.Reset generated when entering Standby mode:
		//		This type of reset is enabled by resetting nRST_STDBY bit in user option bytes. In this
		//		case, whenever a Standby mode entry sequence is successfully executed, the device
		//		is reset instead of entering Standby mode.
		//
		//	2. Reset when entering Stop mode:
		//		This type of reset is enabled by resetting nRST_STOP bit in user option bytes. In this
		//		case, whenever a Stop mode entry sequence


		  result = rer_none;


	}else if  ( rcc_csr & RCC_CSR_IWDGRSTF){
		result = rer_independendWatchdog;
	}else if  ( rcc_csr & RCC_CSR_WWDGRSTF){
		result = rer_windowWatchdog;
	}else if  ( rcc_csr & RCC_CSR_SFTRSTF){
		result = rer_softwareReset;
	} else if  ( rcc_csr & RCC_CSR_PORRSTF){
		 result = rer_powerOnReset;  //works
	} else if  (rcc_csr & RCC_CSR_PINRSTF){
		 result = rer_resetPin;   //works
	} else {
		result = rer_none;
		//shouldnt happen
	}
#endif
	return result;
}

bool port_checkFlashConfiguration(bool usePrintf){
	if (portFlashGetFlashSize() != FLASH_SIZE){
		if(usePrintf){
			printf("Flash size malconfigured");
		}
		return false;
	}

	if (sizeof(firmware_meta_t) > FIRMWARE_DESCRIPTION_BUFFER_SIZE){
		if(usePrintf){
			printf("Application meta data too big for buffer");
		}
		return false;
	}

	uint32_t lastUsedAddressInFlash = (uint32_t)&__firmware_descriptor_buffer_end;
	if (MINIMAL_APPLICATION_ADDRESS < lastUsedAddressInFlash){
		if(usePrintf){
			printf("Bootloader flash region overlaps with application memory");
		}
		return false;
	}
	return true;
}
