/*
 * port_serial.c

 *
 *  Created on: 17.10.2016
 *      Author: ak
 */
#include "port_serial.h"
#include "fifo/fifo.h"
#include "board.h"

#define BUFFER_LENGTH_RX 128
#define BUFFER_LENGTH_TX 128

static fifo_t fifo_tx;
static fifo_t fifo_rx;

static uint8_t fifo_buffer_tx[BUFFER_LENGTH_RX];
static uint8_t fifo_buffer_rx[BUFFER_LENGTH_TX];

void portSerialInit(int baud){
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	fifo_init(&fifo_tx,fifo_buffer_tx,BUFFER_LENGTH_TX);
	fifo_init(&fifo_rx,fifo_buffer_rx,BUFFER_LENGTH_RX);
#if 1
	USART_InitStructure.USART_BaudRate = baud;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	/* Enable UART clock */
	RCC_APB1PeriphClockCmd(COM_USART_CLK, ENABLE);


	/* USART configuration */
	USART_Init(COM_USART_BASE, &USART_InitStructure);


	/* NVIC configuration */
	/* Configure the Priority Group to 2 bits */
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	/* Enable the USARTx Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = COM_USART_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	USART_ITConfig(COM_USART_BASE, USART_IT_RXNE, ENABLE);

	/* Enable USART */
	USART_Cmd(COM_USART_BASE, ENABLE);
#endif
}


/**
 * @brief  Print a character on the HyperTerminal
 * @param  c: The character to be printed
 * @retval None
 */
void portSerialPutChar(uint8_t c)
{
	//USART_SendData(COM_USART_BASE, c);
	fifo_push(&fifo_tx,c);
	USART_ITConfig(COM_USART_BASE, USART_IT_TXE, ENABLE);
	USART_SendData(COM_USART_BASE,c);
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
		fifo_pop(&fifo_rx,c);
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
