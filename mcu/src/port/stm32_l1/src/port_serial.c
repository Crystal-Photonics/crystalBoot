/*
 * port_serial.c

 *
 *  Created on: 17.10.2016
 *      Author: ak
 */
#include <assert.h>
#include "port_serial.h"
#include "fifo/fifo.h"
#include "port_chip.h"
#include "port_board.h"
//#include "main.h"

#define BUFFER_LENGTH_RX 256
#define BUFFER_LENGTH_TX 256

static fifo_t fifo_tx;
static fifo_t fifo_rx;

static uint8_t fifo_buffer_tx[BUFFER_LENGTH_RX];
static uint8_t fifo_buffer_rx[BUFFER_LENGTH_TX];


void portSerialInit(int baud){
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	fifo_init(&fifo_tx,fifo_buffer_tx,BUFFER_LENGTH_TX);
	fifo_init(&fifo_rx,fifo_buffer_rx,BUFFER_LENGTH_RX);

	USART_InitStructure.USART_BaudRate = baud;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	/* Enable UART clock */
	RCC_APB2PeriphClockCmd(COM_USART_CLK, ENABLE);


	/* USART configuration */
	USART_Init(COM_USART_BASE, &USART_InitStructure);



#if 1
	/* NVIC configuration */
	/* Configure the Priority Group to 2 bits */
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	/* Enable the USARTx Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = COM_USART_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = ISR_PRIORITY_SERIAL_DBG;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	USART_ITConfig(COM_USART_BASE, USART_IT_RXNE, ENABLE);
#endif
	/* Enable USART */
	USART_Cmd(COM_USART_BASE, ENABLE);

}

#if 0

uint8_t USART_GetCR1ITConfig(USART_TypeDef* USARTx)
{
/*
	Bit 7 TXEIE: TXE interrupt enable
	Bit 6 TCIE: Transmission complete interrupt enable
	Bit 5 RXNEIE: RXNE interrupt enable
	it 4 IDLEIE: IDLE interrupt enable
	Bit 3 TE: Transmitter enable
	Bit 2 RE: Receiver enable
	Bit 1 RWU: Receiver wakeup
	Bit 0 SBK: Send break
*/
	/* Check the parameters */
	assert_param(IS_USART_ALL_PERIPH(USARTx));


	return USARTx.CR1 & 0xFF;
}

void USART_RestorITConfig(USART_TypeDef* USARTx,uint8_t cr1_it_Mask)
{
	assert_param(IS_USART_ALL_PERIPH(USARTx));
	uint32_t USARTx_CR1 = USARTx.CR1;
	USARTx_CR1 &= 0xFFFFFF00;
	USARTx_CR1 |= cr1_it_Mask;
	USARTx.CR1 = USARTx_CR1;
}
#endif
/**
 * @brief  Print a character on the HyperTerminal
 * @param  c: The character to be printed
 * @retval None
 */
void portSerialPutChar(uint8_t c)
{

	bool wasEmpty = fifo_is_empty(&fifo_tx);

	port_EnterCritical();
	fifo_result_t result = fifo_push(&fifo_tx,c);
	port_ExitCritical();
	//assert(result == FIFO_SUCCESS);
	if (wasEmpty){
		USART_ITConfig(COM_USART_BASE, USART_IT_TXE, ENABLE);
	}
}

/**
 * @brief  Print a string on the HyperTerminal
 * @param  s: The string to be printed
 * @retval None
 */
void portSerialPutString(char *s)
{
	while (*s != '\0')
	{
		portSerialPutChar(*s);
		s++;
	}
}


bool portSerialGetChar(uint8_t *c){

	if (fifo_is_empty(&fifo_rx)){
		return false;
	}else{
		port_EnterCritical();
		fifo_pop(&fifo_rx,c);
		port_ExitCritical();
		return true;
	}
}


/******************************************************************************/
/*            STM32L1xx Peripherals Interrupt Handlers                        */
/******************************************************************************/
/**
* @brief  This function handles USRAT interrupt request.
* @param  None
* @retval None
*/
void USART1_IRQHandler(void)
{
	/* USART in mode Tramitter -------------------------------------------------*/
	if (USART_GetITStatus(COM_USART_BASE, USART_IT_TXE) == SET)
	{
		/* Send the data */
		if (fifo_is_empty(&fifo_tx)){
			USART_ITConfig(COM_USART_BASE, USART_IT_TXE, DISABLE);
		}else{
			uint8_t byte;
			fifo_pop(&fifo_tx,&byte);
			USART_SendData(COM_USART_BASE,byte);
		}
	}

	/* USART in mode Receiver --------------------------------------------------*/
	if (USART_GetITStatus(COM_USART_BASE, USART_IT_RXNE) == SET)
	{
		fifo_push(&fifo_rx,USART_ReceiveData(COM_USART_BASE));
	}
}
