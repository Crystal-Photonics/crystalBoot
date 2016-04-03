
#include "board.h"


void initPort(void){
	PORTD = 0;
	PORTB = 0;//PULLUP
	PORTA = 0;//PULLUP
	DDRA = (uint8_t)~(PIN_SWITCH_L | PIN_TCM | PIN_TCP | PIN_SWITCH_M | PIN_SWITCH_R | PIN_POLARITY);
	DDRB = (uint8_t)~(PIN_RX | PIN_MISO);
	DDRC = 0;
	DDRD = 0xFF;



	MCUCR = (1 << PUD); //pullup off
	//CLEAR_BTONOFF();
}

