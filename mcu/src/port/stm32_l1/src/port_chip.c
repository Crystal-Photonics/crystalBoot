
#include "port_chip.h"
#include "port_board.h"

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
