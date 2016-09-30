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


#ifndef INCLUDE_TASK_KEY_H_
#define INCLUDE_TASK_KEY_H_

#include "board.h"

typedef enum {ke_none,ke_pressShort,ke_pressLong, ke_release} key_event_t;

typedef void (*keyPressHandle_t) (key_event_t, key_id_t);

void keyInit(void);

//returns true if successful
bool keyRegisterHandle(keyPressHandle_t h);

//returns true if successful
bool keyUnRegisterHandle(keyPressHandle_t h);
void taskKey( void *pvParameters );

#endif /* INCLUDE_TASK_KEY_H_ */

