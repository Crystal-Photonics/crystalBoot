/*
 * max6675.c
 *
 *  Created on: 15.07.2013
 *      Author: arne
 */

#include "board.h"

#include "max6675.h"

void max6675Init(){
#if 1
	SET_SPI_CS();
	SPCR = (1 << SPE) | (1<<MSTR) | (0 << CPOL) | (0 << CPHA);
#endif
}

int8_t max6675Read(int16_t *temp){
	uint8_t meta=0;
	CLEAR_SPI_CS();
	*temp = 0;
	SPDR = 0;
	while ((SPSR & (1 << SPIF)) == 0){}
	*temp |= (SPDR << 8);
	SPDR = 0;
	while ((SPSR & (1 << SPIF)) == 0){}
	*temp |= SPDR;

	//*temp = 0x8004;
	meta = 0;
	meta = 7 & *temp;
	meta |= *temp >> 12; //dummy signbit
	meta &= 0x0F;
	*temp = *temp >> 3;
	//if D2 high when thermocouple open
	SET_SPI_CS();
	//return meta;
	if (meta & 4) return -1; //its open
	if (meta & 8) return -2; //dummy bit is 1

	return 0;
}
