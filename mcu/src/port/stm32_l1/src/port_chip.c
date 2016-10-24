
#include "port_chip.h"
#include "port_board.h"


const uint32_t MAGIC_KEY_IN_BACKUP_TO_START_DIRECTLY_IN_APPMODE = 0x5868FE4A;
const uint32_t BACKUPADDRESS_OF_DIRECTLY_START_IN_APPMODE_VALUE = RTC_BKP_DR31;

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

bool port_isDirectApplicationLaunchProgrammed(){
	uint32_t val = mainBKUPGetSetting(BACKUPADDRESS_OF_DIRECTLY_START_IN_APPMODE_VALUE,0xFFFFFFFF,0);
	mainBKUPSetSetting(0,BACKUPADDRESS_OF_DIRECTLY_START_IN_APPMODE_VALUE,0xFFFFFFFF,0);
	return val == MAGIC_KEY_IN_BACKUP_TO_START_DIRECTLY_IN_APPMODE;
}

void port_programDirectApplicationLaunch(void){
	mainBKUPSetSetting(MAGIC_KEY_IN_BACKUP_TO_START_DIRECTLY_IN_APPMODE,
			BACKUPADDRESS_OF_DIRECTLY_START_IN_APPMODE_VALUE,
			0xFFFFFFFF,
			0);
}
