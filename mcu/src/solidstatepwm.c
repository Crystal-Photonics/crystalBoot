/*
 * solidstatepwm.c
 *
 *  Created on: 15.12.2013
 *      Author: arne
 */

#include "board.h"
#include "solidstatepwm.h"
#include <avr/interrupt.h>

typedef enum{phase_none,phase_SwitchPos0,phase_SwitchPos1} phaseposition_t;

const uint8_t TIME_OFFSET=120;
const uint8_t HALF_PERIOD = 140;
phaseposition_t phaseposition = phase_none;

uint8_t ledstatus;

uint8_t pwmValue=8;
const uint8_t PWM_MAX=8;

uint8_t pwmPhaseIndex=0;




SIGNAL( TIMER0_COMPA_vect )
{
	TCCR0B = 0;
	TCNT0 = 0x00;
	switch(phaseposition){
		case phase_SwitchPos0:
			OCR0A = HALF_PERIOD;

			if (pwmPhaseIndex >= pwmValue){
				CLEAR_SOLID_STATE();
			}else{
				SET_SOLID_STATE();
			}
			pwmPhaseIndex++;
			phaseposition = phase_SwitchPos1;
			TCCR0B = _BV(CS02) | _BV(CS00);
			TIMSK0 = _BV(OCIE0A);
			break;
		case phase_SwitchPos1:


			if (pwmPhaseIndex >= pwmValue){
				CLEAR_SOLID_STATE();
			}else{
				SET_SOLID_STATE();
			}
			pwmPhaseIndex++;
			phaseposition = phase_none;
			OCR0A = TIME_OFFSET;
			break;

		case phase_none:

			break;
	}
	if (pwmPhaseIndex >= PWM_MAX){
		pwmPhaseIndex=0;
	}
}



SIGNAL( PCINT0_vect )
{


	if(GET_POLARITY()){
		//CLEAR_SOLID_STATE();
		SET_LED_R();
		TCCR0B = 0;
		TCNT0=0x00;
		phaseposition = phase_SwitchPos0;

		OCR0A = TIME_OFFSET;
		TIMSK0 = _BV(OCIE0A);

		TCCR0B = _BV(CS02) | _BV(CS00);


	}else{
		CLEAR_LED_R();
	}
}


void spwm_init(void){
	TCCR0A = 0;
	TCCR0B = 0;
	spwm_setVal(0);
	CLEAR_SOLID_STATE();

	PCMSK0 =_BV(PCINT7);
	PCICR = _BV(PCIE0);
}

void spwm_setVal(uint8_t val){
	if (val > PWM_MAX)
		val = PWM_MAX;
	pwmValue  = val;
}
