
#include "globals.h"
#include "serial.h"
#include "board.h"
#include <avr/interrupt.h>


/* Constants for writing to UCSRB. */
#define serRX_INT_ENABLE				( ( uint8_t ) _BV(RXCIE0) )
#define serRX_ENABLE					( ( uint8_t ) _BV(RXEN0))
#define serTX_ENABLE					( ( uint8_t ) _BV(TXEN0) )
#define serTX_INT_ENABLE				( ( uint8_t ) _BV(TXCIE0) )

/* Constants for writing to UCSRC. */
#define serEIGHT_DATA_BITS				( ( uint8_t ) 0x06 )


#define vInterruptOnRXOFF()									\
{															\
	uint8_t ucByte;									\
															\
	ucByte = UCSR0B;											\
	ucByte |= serTX_INT_ENABLE;								\
								\
	ucByte &= ~serRX_INT_ENABLE;							\
	UCSR0B = ucByte;											\
}																				
/*-----------------------------------------------------------*/

#define vInterruptOffRXON()									\
{															\
	uint8_t ucInByte;									\
															\
	ucInByte = UCSR0B;										\
	ucInByte &= ~serTX_INT_ENABLE;							\
									\
	ucInByte |= serRX_INT_ENABLE;							\
	UCSR0B = ucInByte;										\
}
/*-----------------------------------------------------------*/

uint8_t PlaceOnQueue( uint8_t cOutChar);

uint8_t GetFromOnQueue( uint8_t *pcRxedChar);

uint8_t iSerialGetQueuedLength(void);
	
void xSerialPortInitMinimal( uint16_t ulWantedBaud)
{
	uint8_t ucByte;
	uint8_t SREGBackUp;
	
	SREGBackUp = SREG;
	cli();
	{
		fifo_init(&fifoRX, SerialBufferRX, SERIAL_BUFFER_SIZE_RX);
		fifo_init(&fifoTX, SerialBufferTX, SERIAL_BUFFER_SIZE_TX);
		/* Create the queues used by the com test task. */

		/* Calculate the baud rate register value from the equation in the
		data sheet. */

		// Set the baud rate. 	
		ucByte = ( uint8_t ) ( ulWantedBaud & ( uint32_t ) 0xff );	
		UBRR0L = ucByte;

		ucByte = ( uint8_t ) ( ulWantedBaud >> 8 & ( uint32_t ) 0xff );	
		UBRR0H = ucByte;

		/* Enable the Rx interrupt.  The Tx interrupt will get enabled
		later. Also enable the Rx and Tx. */
		UCSR0B = ( serRX_INT_ENABLE  | serRX_ENABLE | serTX_ENABLE );
	//	UCSRB = (  serRX_ENABLE | serTX_ENABLE );

		/* Set the data bits to 8. */
		UCSR0C = (  serEIGHT_DATA_BITS );
	}
	SREG = SREGBackUp;
}
/*
	void vSerialPutStringPolling(uint8_t *c ,uint8_t max)
	{
		uint8_t ucInByte,ucSaveByte;								
	
		uint8_t SREGBackUp;
		SREGBackUp = SREG;	
		cli();																					
		ucInByte = UCSRB;	
		ucSaveByte = ucInByte;									
		ucInByte &= ~serTX_INT_ENABLE;	
		ucInByte &= ~serRX_ENABLE;
		ucInByte &= ~serRX_INT_ENABLE;						
		UCSRB = ucInByte;										
	 	while((max>0)&&(*c!='\0'))
	 	{ 
		    loop_until_bit_is_set( UCSRA, UDRE );
		    UDR = *c;
		    c++;
		    max--;
	 	}
		UCSRB = ucSaveByte;
		SREG = SREGBackUp;
	}
	


*/
	
uint8_t xSerialGetChar(uint8_t *pcRxedChar)
{
	uint8_t res;
	res = fifo_get_nowait(&fifoRX,pcRxedChar);
	return res;

}
/*-----------------------------------------------------------*/




	
uint8_t xSerialPutChar( uint8_t cOutChar)
{
	uint8_t result;
	
	if ((fifoTX.count == 0) && (( UCSR0A & _BV(UDRE0))!=0)){
		UDR0 = cOutChar;
		result = 1;
	}else{
		result = fifo_put (&fifoTX, cOutChar);
		vInterruptOnRXOFF();
	}
	return result;
}
/*-----------------------------------------------------------*/


SIGNAL( USART0_RX_vect )
{
uint8_t cChar;

	/* Get the character and post it on the queue of Rxed characters.
	If the post causes a task to wake force a context switch as the woken task
	may have a higher priority than the task we have interrupted. */

	cChar = UDR0;
	 fifo_put (&fifoRX, cChar);

}
/*-----------------------------------------------------------*/

SIGNAL( USART0_TX_vect )
{
uint8_t cChar;

	if( fifo_get_nowait(&fifoTX,&cChar)==  TRUE )
	{
		/* Send the next character queued for Tx. */
		UDR0 = cChar;
	}
	else
	{
		/* Queue empty, nothing to send. */
		vInterruptOffRXON();
	}

}
