#ifndef SERIAL_H_
#define SERIAL_H_

#include "fifo.h"
#include <stdint.h>

#define serBAUD_DIV_CONSTANT			( ( uint32_t ) 16 )

#define serBAUD19200					(( F_OSC / ( serBAUD_DIV_CONSTANT * 19200 ) ) - ( uint32_t ) 1)
#define serBAUD115200					(( F_OSC / ( serBAUD_DIV_CONSTANT * 115200 ) ) - ( uint32_t ) 1)

#define SERIAL_BUFFER_SIZE_TX 64
#define SERIAL_BUFFER_SIZE_RX 64
uint8_t FiFoStart,FiFoEnd;



fifo_t fifoRX;
fifo_t fifoTX;

uint8_t SerialBufferTX[SERIAL_BUFFER_SIZE_TX];
uint8_t SerialBufferRX[SERIAL_BUFFER_SIZE_RX];

void xSerialPortInitMinimal( uint16_t ulWantedBaud);
void	vSerialPutStringPolling(uint8_t *c ,uint8_t max);
uint8_t xSerialPutChar( uint8_t cOutChar);
uint8_t xSerialGetChar(uint8_t *pcRxedChar);
uint8_t GetFromOnQueue( uint8_t *pcRxedChar);

#endif /*SERIAL_H_*/
