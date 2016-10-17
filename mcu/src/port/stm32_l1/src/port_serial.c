/*
 * port_serial.c

 *
 *  Created on: 17.10.2016
 *      Author: ak
 */
#include "port_serial.h"
#include "board.h"


/**
 * @brief  Print a character on the HyperTerminal
 * @param  c: The character to be printed
 * @retval None
 */
void SerialPutChar(uint8_t c)
{
	USART_SendData(EVAL_COM1, c);
	while (USART_GetFlagStatus(EVAL_COM1, USART_FLAG_TXE) == RESET)
	{
	}
}

/**
 * @brief  Print a string on the HyperTerminal
 * @param  s: The string to be printed
 * @retval None
 */
void SerialPutString(char *s)
{
	while (*s != '\0')
	{
		SerialPutChar(*s);
		s++;
	}
}
