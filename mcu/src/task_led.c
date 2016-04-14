/*
 *    Copyright (C) 2015  Crystal-Photonics GmbH
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */



#include "board.h"
#include "task_led.h"
#include "serial.h"
#include <stdio.h>
#include <string.h>





ledBlinkPattern_t ledPatterns[lpp_priority_COUNT-1][lid_LEDCOUNT-1];

ledPatternPriority_t getMaxPatternPriority(led_id_t ledID){
	ledPatternPriority_t result = lpp_priority_1;
	for (int prio = 1; prio < lpp_priority_COUNT; prio++ ){
		if (ledPatterns[prio-1][ledID-1].active){
			result = prio;
			break;
		}

	}
	return result;
}

void ledPatternSetToFade(led_id_t ledID, ledPatternPriority_t priority){
	if (ledID < lid_LEDCOUNT){
		ledPatterns[priority-1][ledID-1].active = true;
		ledPatterns[priority-1][ledID-1].patternStyle = LED_BLINK_PATTERNSTYLE_FADE;
		ledPatterns[priority-1][ledID-1].ledID = ledID;
		ledPatterns[priority-1][ledID-1].actionCounter = 0;
	}
}

void ledPatternSetToMorse(led_id_t ledID, ledPatternPriority_t priority, uint8_t morsePattern, uint8_t morsePatternLength){
	if ((ledID < lid_LEDCOUNT) && (9 > morsePatternLength) && (morsePatternLength > 0)){
		ledPatterns[priority-1][ledID-1].ledID = ledID;
		ledPatterns[priority-1][ledID-1].active = true;
		ledPatterns[priority-1][ledID-1].patternStyle = morsePatternLength;
		ledPatterns[priority-1][ledID-1].morsePattern = morsePattern;
		ledPatterns[priority-1][ledID-1].timeOff50ms = 2;
		ledPatterns[priority-1][ledID-1].timeOn50ms = 2;
		ledPatterns[priority-1][ledID-1].timePause50ms = 50;
		ledPatterns[priority-1][ledID-1].actionCounter = 0;
	}
}

uint8_t ledGetPatternCode(led_id_t ledID){
	if (ledID < lid_LEDCOUNT)
		return ledPatterns[getMaxPatternPriority(ledID)-1][ledID-1].morsePattern;
	else
		return 0;
}

void ledPatternSetToBlink(led_id_t ledID, ledPatternPriority_t priority, uint8_t timeOn, uint8_t timeOff, uint8_t timePause, uint8_t repeat){
	if (ledID < lid_LEDCOUNT){
		ledPatterns[priority-1][ledID-1].ledID = ledID;
		ledPatterns[priority-1][ledID-1].active = true;
		ledPatterns[priority-1][ledID-1].patternStyle = LED_BLINK_PATTERNSTYLE_BLINK;
		ledPatterns[priority-1][ledID-1].timeOff50ms = timeOff;
		ledPatterns[priority-1][ledID-1].timeOn50ms = timeOn;
		ledPatterns[priority-1][ledID-1].timePause50ms = timePause;
		ledPatterns[priority-1][ledID-1].repeat = repeat;
		ledPatterns[priority-1][ledID-1].actionCounter = 0;
	}
}

void ledPatternSetToOff(led_id_t ledID,ledPatternPriority_t priority){
	if (ledID < lid_LEDCOUNT ){
		memset(&ledPatterns[priority-1][ledID-1],sizeof(ledBlinkPattern_t),0);
		ledPatterns[priority-1][ledID-1].active = true;
		ledPatterns[priority-1][ledID-1].patternStyle = LED_BLINK_PATTERNSTYLE_BLINK;
		ledPatterns[priority-1][ledID-1].timeOff50ms = 0;
		ledPatterns[priority-1][ledID-1].timeOn50ms = 0;
		ledPatterns[priority-1][ledID-1].timePause50ms = 0;
		ledPatterns[priority-1][ledID-1].repeat = 0;
		ledPatterns[priority-1][ledID-1].ledID = ledID;
		boardSetLED(ledID,les_off);
	}
}

void ledPatternDeactivate(led_id_t ledID,ledPatternPriority_t priority){
	if (ledID < lid_LEDCOUNT ){
		memset(&ledPatterns[priority-1][ledID-1],sizeof(ledBlinkPattern_t),0);
		ledPatterns[priority-1][ledID-1].active = false;
		ledPatterns[priority-1][ledID-1].patternStyle = LED_BLINK_PATTERNSTYLE_BLINK;
		ledPatterns[priority-1][ledID-1].timeOff50ms = 0;
		ledPatterns[priority-1][ledID-1].timeOn50ms = 0;
		ledPatterns[priority-1][ledID-1].timePause50ms = 0;
		ledPatterns[priority-1][ledID-1].repeat = 0;
		ledPatterns[priority-1][ledID-1].ledID = ledID;
		boardSetLED(ledID,les_off);
	}
}

#if 0
void ledPatternBackUp(led_id_t ledID,ledPatternPriority_t priority, ledBlinkPattern_t *pattern){
	if ((ledID < lid_LEDCOUNT) && (pattern)){
		memcpy(pattern,&ledPatterns[ledID-1], sizeof(ledBlinkPattern_t));
	}
}

void ledPatternRestore(led_id_t ledID, ledPatternPriority_t priority, ledBlinkPattern_t *pattern){
	if ((ledID < lid_LEDCOUNT) && (pattern)){
		memcpy(&ledPatterns[ledID-1], pattern, sizeof(ledBlinkPattern_t));
	}
}
#endif

void ledTick(ledBlinkPattern_t *p){
	int action;
	if (p->nextActionTime)
		p->nextActionTime--;

	if (p->nextActionTime==0){
		switch(p->patternStyle){
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		{
			// 0 = on(short)
			// 1 = on(long)
			// 2 = off
			// 3 = pause

			int bitIndex = p->actionCounter/2;
			p->actionCounter++;
			if (bitIndex<3){
				action = 1;
			}else{
				bitIndex-=3;
				if ((p->morsePattern >> bitIndex) & 1){
					action = 0;
				}else{
					action = 1;

				}
				if (bitIndex >= p->patternStyle){
					action = 3;
					p->actionCounter = 0;
				}
			}
			if (((p->actionCounter & 1)==0) && (action != 3)){
				action = 2;
			}
			switch(action){
			case  0:
				boardSetLED(p->ledID,les_on);
				p->nextActionTime = p->timeOn50ms;
				break;
			case 1:
				boardSetLED(p->ledID,les_on);
				p->nextActionTime = p->timeOn50ms*16;
				break;
			case 2:
				boardSetLED(p->ledID,les_off);
				p->nextActionTime = p->timeOff50ms;
				break;
			case 3:
				boardSetLED(p->ledID,les_off);
				p->nextActionTime = p->timePause50ms;
				p->patternCounter++;
			}

		}
		break;
		case LED_BLINK_PATTERNSTYLE_BLINK:
			//actionCounter = 0: 			led einschalten und timeOn50ms warten
			//actionCounter = 1: 			led auschalten und timeOff50ms warten

			//actionCounter = 2: 			led einschalten
			//actionCounter = 3: 			led auschalten

			//actionCounter = reapeat*2: 	led auschalten und timePause50ms warten
			p->actionCounter++;

			if ((p->actionCounter & 1) == 0){
				action = 1; //off
			}
			if ((p->actionCounter & 1) == 1){
				action = 2; //on
				if (p->timeOn50ms == 0){
					action = 1;
				}
			}
			if (p->actionCounter == p->repeat*2){
				action = 0;
				p->actionCounter = 0;
			}
			if (p->repeat == 0){
				action = 1;
			}

			switch(action){
			case 0: 	boardSetLED(p->ledID,les_off);
						p->nextActionTime = p->timePause50ms;
						p->patternCounter++;
						break;
			case 1: 	boardSetLED(p->ledID,les_off);
						p->nextActionTime = p->timeOff50ms;
						break;
			case 2: 	boardSetLED(p->ledID,les_on);
						p->nextActionTime = p->timeOn50ms;
						break;
			}

			break;
			case LED_BLINK_PATTERNSTYLE_FADE:
			{
				uint8_t ledValue;
				if ((p->actionCounter & 0x8000) == 0){
					p->actionCounter += 0x10;
					if ((p->actionCounter & 0x7F00) > 0x0F00){
						p->actionCounter |= 0x8000;
						p->patternCounter++;
					}

				}else{
					p->actionCounter -= 0x10;
					p->actionCounter |= 0x8000;
					if ((p->actionCounter & 0x7F00) == 0x0000)
						p->actionCounter &= 0x7FFF;
				}

				action = p->actionCounter & 0x03;
				action += 1;
				action &= 0x03;
				if (action == 4)
					action = 0;
				p->actionCounter &= 0xFFFC;
				p->actionCounter |= action;
				p->nextActionTime = 1 << action;
				ledValue = p->actionCounter >> 8;
				if (ledValue & p->nextActionTime){
					boardSetLED(p->ledID,les_on);
				}else{
					boardSetLED(p->ledID,les_off);
				}
			}
			break;
		}
	}
}
#if 0
void ledInitPattern(ledBlinkPattern_t *p, led_id_t id){
	memset(p,sizeof(ledBlinkPattern_t),0);
	p->ledID = id;
	p->timeOn50ms = 1;
	p->timeOff50ms = 1;
	p->timePause50ms = 1;
	p->repeat = 2;
	boardSetLED(id,les_off);
}
#endif
void taskLEDPatternLoop( delayFunction_t delayFunction, ledLoopRestartHandler_t ledLoopRestartHandler ){
	for( ;; )
	{
		bool containsFadePatterns=false;
		for (int i = 1; i < lid_LEDCOUNT; i++){
			ledPatternPriority_t maxPrio = getMaxPatternPriority(i);
			if (ledPatterns[maxPrio-1][i-1].patternStyle == LED_BLINK_PATTERNSTYLE_FADE)
				containsFadePatterns = true;
		}

		if (containsFadePatterns){
			for (int n=0;n<50;n++){
				delayFunction(( 1 / portTICK_RATE_MS ));
				for (int i = 1; i < lid_LEDCOUNT; i++){
					ledPatternPriority_t maxPrio = getMaxPatternPriority(i);
					if (ledPatterns[maxPrio-1][i-1].patternStyle == LED_BLINK_PATTERNSTYLE_FADE)
						ledTick(&ledPatterns[maxPrio-1][i-1]);
				}
			}
			for (int i = 1; i < lid_LEDCOUNT; i++){
				ledPatternPriority_t maxPrio = getMaxPatternPriority(i);
				if (ledPatterns[maxPrio-1][i-1].patternStyle != LED_BLINK_PATTERNSTYLE_FADE)
					ledTick(&ledPatterns[maxPrio-1][i-1]);
			}
		}else{
			delayFunction(( 50 / portTICK_RATE_MS ));
			for (int i = 1; i < lid_LEDCOUNT; i++){
				ledPatternPriority_t maxPrio = getMaxPatternPriority(i);
				ledTick(&ledPatterns[maxPrio-1][i-1]);
			}
		}
		if (ledLoopRestartHandler)
			ledLoopRestartHandler();
	}
}

uint8_t ledGetPatternCounter(led_id_t ledID ){
	ledPatternPriority_t maxPrio = getMaxPatternPriority(ledID);
	return ledPatterns[maxPrio-1][ledID-1].patternCounter;
}



void ledLoopRestartHandler( ){

}

void ledStandAloneWaiter(TickType_t waitForTicks ){
	while(waitForTicks--){
		for (int i=0; i<2500; i++);
		{

		}
	}
}

void taskLEDRunStandAloneLEDPatternLoop(  ledLoopRestartHandler_t  ledLoopRestartHandler ){
	for (int ledindex = 1; ledindex < lid_LEDCOUNT; ledindex++){
		ledPatternPriority_t maxPrio = getMaxPatternPriority(ledindex);
		ledPatterns[maxPrio-1][ledindex-1].patternCounter = 0;
	}
	taskLEDPatternLoop(ledStandAloneWaiter,ledLoopRestartHandler);
}


void taskLED( void *pvParameters )
{
#if 1

	for (int prio = 1; prio < lpp_priority_COUNT; prio++){
		for (int ledindex = 1; ledindex < lid_LEDCOUNT; ledindex++){
			ledPatternSetToOff(prio,ledindex);
			ledPatternDeactivate(prio,ledindex);
		}
	}
	taskLEDPatternLoop(vTaskDelay,ledLoopRestartHandler);
#else
	for (;;){
		static uint8_t ledstatus = 0;
		if (ledstatus & 1){
			SET_LED_BLUE();
			SET_LED_RED();
		}else{
			CLEAR_LED_BLUE();
			CLEAR_LED_RED();
		}
		ledstatus++;
		vTaskDelay(( 50 / portTICK_RATE_MS ));
	}
#endif
}

