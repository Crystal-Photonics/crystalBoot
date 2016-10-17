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

#ifndef TASK_LED_H_
#define TASK_LED_H_


#include "board.h"


#define LED_BLINK_PATTERNSTYLE_BLINK 0
#define LED_BLINK_PATTERNSTYLE_FADE 9

typedef enum{lpp_none, lpp_priority_0_max, lpp_priority_1, lpp_priority_COUNT} ledPatternPriority_t;

typedef struct{
	bool active;
	led_id_t ledID;
	uint8_t patternStyle;
	//wenn 0 einfaches Blinken, definiert durch ein/auszeiten.
		//LED Ein | LED Aus | LED Ein | LED Aus | ..repeat.. | Pause | LED Ein | LED Aus | LED Ein | LED Aus | ..repeat.. | Pause |

	//wenn 1-8 Morsepattern mit 1-8 Bits und mit timeOff50ms als Länge zwischen den Bits
		//0 LED ein für 2*timeOn50ms
		//1 LED ein für timeOn50ms
		//LED Pattern | Pause | ..repeat.. |

	//wenn 9 dann auf/abschwellen der LED

	uint8_t morsePattern;
	uint8_t timeOn50ms;		//wenn 0, dann LED aus
	uint8_t timeOff50ms;
	uint16_t timePause50ms;
	uint8_t repeat;			//wenn 0, dann LED aus

	uint16_t actionCounter;
	uint16_t nextActionTime;

	uint8_t patternCounter;	//increments, when pause is played
} ledBlinkPattern_t;



typedef void (*delayFunction_t)(TickType_t);
typedef void (*ledLoopRestartHandler_t)(void);

void taskLED( void *pvParameters );
void ledPatternSetToMorse(led_id_t ledID, ledPatternPriority_t priority, uint8_t morsePattern, uint8_t morsePatternLength);
void ledPatternSetToBlink(led_id_t ledID, ledPatternPriority_t priority, uint8_t timeOn, uint8_t timeOff, uint8_t timePause, uint8_t repeat);
void ledPatternSetToFade(led_id_t ledID, ledPatternPriority_t priority);
void ledPatternSetToOff(led_id_t ledID, ledPatternPriority_t priority);

void ledPatternDeactivate(led_id_t ledID,ledPatternPriority_t priority);
#if 0
void ledPatternBackUp(led_id_t ledID, ledPatternPriority_t priority, ledBlinkPattern_t *pattern);
void ledPatternRestore(led_id_t ledID, ledPatternPriority_t priority, ledBlinkPattern_t *pattern);
#endif

uint8_t ledGetPatternCode(led_id_t ledID);
uint8_t ledGetPatternCounter(led_id_t ledID );

void taskLEDRunStandAloneLEDPatternLoop( ledLoopRestartHandler_t  ledLoopRestartHandler);


#endif /* TASK_LED_H_ */
