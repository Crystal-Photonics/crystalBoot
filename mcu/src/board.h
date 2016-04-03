#ifndef BOARD_H_
#define BOARD_H_

#include <avr/io.h>
#include <avr/interrupt.h>

//ADC

#define	PIN_TCM			(1<<0)	//out
#define	PIN_TCP			(1<<1)	//out

#define	PIN_SWITCH_L			(1<<5)	//out
#define GET_SWITCH_L()			(PINA & PIN_SWITCH_L)

#define	PIN_SWITCH_M			(1<<4)	//out
#define GET_SWITCH_M()			(PINA & PIN_SWITCH_M)

#define	PIN_SWITCH_R			(1<<2)	//out
#define GET_SWITCH_R()			(PINA & PIN_SWITCH_R)


#define	PIN_POLARITY			(1<<7)	//out
#define GET_POLARITY()			(PINA & PIN_POLARITY)

#define	PIN_LED_L				(1<<6)	//out
#define SET_LED_L()				(PORTA |= PIN_LED_L)
#define CLEAR_LED_L()			(PORTA &= ~PIN_LED_L)

#define	PIN_LED_R				(1<<0)	//out
#define SET_LED_R()				(PORTB |= PIN_LED_R)
#define CLEAR_LED_R()			(PORTB &= ~PIN_LED_R)


#define PIN_MISO					(1<<6)	//in
#define GET_MISO()				(PINB & PIN_RX)

#define	PIN_SPI_CS				(1<<4)	//out
#define SET_SPI_CS()			(PORTB |= PIN_SPI_CS)
#define CLEAR_SPI_CS()			(PORTB &= ~PIN_SPI_CS)

#define	PIN_LED_FAN				(1<<3)	//out
#define SET_LED_FAN()			(PORTB |= PIN_LED_FAN)
#define CLEAR_LED_FAN()			(PORTB &= ~PIN_LED_FAN)


#define	PIN_SOLID_STATE			(1<<2)	//out
#define SET_SOLID_STATE()		(PORTB |= PIN_SOLID_STATE)
#define CLEAR_SOLID_STATE()		(PORTB &= ~PIN_SOLID_STATE)

#define	PIN_BUZZER				(1<<1)	//out
#define SET_BUZZER()			(PORTB |= PIN_BUZZER)
#define CLEAR_BUZZER()			(PORTB &= ~PIN_BUZZER)




//Port D
#define PIN_RX					(1<<0)	//in
#define GET_RX()				(PIND & PIN_RX)



#define PIN_TX					(1<<1)	//out
#define SET_TX()				(PORTD |= PIN_TX)
#define CLEAR_TX()				(PORTD &= ~PIN_TX)




#define	PIN_LED_M				(1<<3)	//out
#define SET_LED_M()				(PORTA |= PIN_LED_M)
#define CLEAR_LED_M()			(PORTA &= ~PIN_LED_M)


void initPort(void);



#endif /*BOARD_H_*/
