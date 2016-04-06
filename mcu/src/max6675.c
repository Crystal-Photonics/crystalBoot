/*
 * max6675.c
 *
 *  Created on: 15.07.2013
 *      Author: arne
 */

#include "board.h"
#include <string.h>

#include "max6675.h"

void max6675Init(){
#if 1
	SET_SPI_CS();
	SPCR = (1 << SPE) | (1<<MSTR) | (0 << CPOL) | (0 << CPHA) | (1 << SPR1) | (1 << SPR0);
#endif
}

int8_t max6675Read(max33185result_t *maxResult){
	uint32_t inData=0;
	memset(maxResult,0,sizeof(maxResult));
	CLEAR_SPI_CS();
	SPDR = 0;
	while ((SPSR & (1 << SPIF)) == 0){}
	inData |= ((uint32_t)SPDR << 24);
	SPDR = 0;
	while ((SPSR & (1 << SPIF)) == 0){}
	inData |= ((uint32_t)SPDR << 16);


	SPDR = 0;
	while ((SPSR & (1 << SPIF)) == 0){}
	inData |= ((uint32_t)SPDR << 8);
	SPDR = 0;
	while ((SPSR & (1 << SPIF)) == 0){}
	inData |= (SPDR << 0);


	if (inData & 0x0001)
		maxResult->ocFault = 1;

	if (inData & 0x0002)
		maxResult->scgFault = 1;

	if (inData & 0x0004)
		maxResult->scvFault = 1;

	if (inData & 0x0100)
		maxResult->fault = 1;

	maxResult->temperature_thermocuouple = inData >> 18;

	maxResult->temperature_coldjunction = inData >> 4;
	maxResult->temperature_coldjunction &= 0x0FFF;

	if (inData & 0x8000)
		maxResult->temperature_coldjunction |= 0xF000;

	if (inData & 0x80000000)
		maxResult->temperature_coldjunction |= 0xC000;


	SET_SPI_CS();
	//return meta;
	if (maxResult->fault || maxResult->scvFault || maxResult->scgFault || maxResult->ocFault) return -1;


	return 0;
}
