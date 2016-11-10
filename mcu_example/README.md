# crystal Boot example
uses interrupts.

parts to have a look at:

STM32L151XE_FLASH_bootloader.ld :

Defines flash start address above bootloader:
```
MEMORY
{
/*FLASH (rx)      : ORIGIN = 0x8005000, LENGTH = 488K*/
FLASH (rx)      : ORIGIN = 0x8005000, LENGTH = 488K
RAM (xrw)       : ORIGIN = 0x20000000, LENGTH = 80K
}
```
In section isr_vector you should export the interrupt vector address to the variable _start_isr_vector. Then the startupcode will fetch the vectortable address automatically.
```
SECTIONS
{
  /* The startup code goes first into FLASH */
  .isr_vector :
  {
    . = ALIGN(4);
    PROVIDE(_start_isr_vector = .);
```	
	
In system_stm32l1xx.c normally the vectortable position is initialized to 0. This wont work in a bootloader environment. Therefore you should better use something like this:

```
  volatile uint32_t ISR_VECTOR_TABLE_OFFSET = (uint32_t)&_start_isr_vector;

  ISR_VECTOR_TABLE_OFFSET = ISR_VECTOR_TABLE_OFFSET - FLASH_BASE;

  SCB->VTOR = FLASH_BASE | ISR_VECTOR_TABLE_OFFSET; /* Vector Table Relocation in Internal FLASH. */
  
```  
  
The bootloader disables interrupts before starting application. Therefore you should call 

```

	__enable_irq();			//interrupts are disabled
```
	
in main().