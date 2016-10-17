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

#ifndef INCLUDE_TASK_LOWPOWER_H_
#define INCLUDE_TASK_LOWPOWER_H_

#define RTC_STATE_OFF                    0
#define RTC_STATE_ON                     1

#define BIAS_CALIB_OFF                   0
#define BIAS_CALIB_ON                    1

#define STM32L_MODE_RUN                  0
#define STM32L_MODE_LP                   1
#define STM32L_MODE_BIAS                 2

#define STM32L_RUN                       0
#define STM32L_RUN_1M                    1
#define STM32L_RUN_LP                    2
#define STM32L_SLEEP                     3
#define STM32L_SLEEP_LP                  4
#define STM32L_STOP                      5
#define STM32L_STANDBY                   6









void lowPowerInitPeriodicWakeUp(uint16_t period_s);
void lowPowerStartLP(uint32_t lpmode, uint8_t RTCState);

//bool lowPowerAllowStandBy(standbyAllowFlagIndex_t flagIndex,standbyAction_t standbyAction_, bool allow);
void lowPowerInitAllowStandBy(void);

void lowPowerSwitchSystemOff( void  );
void lowPowerSwitchSystemOffButTestReason( void  );
#endif /* INCLUDE_TASK_LOWPOWER_H_ */
