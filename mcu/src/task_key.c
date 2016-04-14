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

#include "task_key.h"
#include "task.h"

#define HANDLELIST_SIZE 8

//in 50ms
#define KEYPRESS_LONG_THRESHOLD 20

keyPressHandle_t keyHandleList[HANDLELIST_SIZE];

unsigned int keyList[kid_KEYCOUNT];

void keyInit(void){
	int i;
	for(i=0;i<HANDLELIST_SIZE;i++){
		keyHandleList[i] = NULL;
	}
	for(i=0;i<kid_KEYCOUNT;i++){
		keyList[i] = 0;
	}

}

int findHandle(keyPressHandle_t h){
	int result = -1;
	int i;
	for(i=0;i<HANDLELIST_SIZE;i++){
		if (keyHandleList[i] == h){
			result = i;
			break;
		}
	}
	return result;
}

//returns true if succesfull
bool keyRegisterHandle(keyPressHandle_t h){
	int i;
	keyUnRegisterHandle(h);
	i = findHandle(NULL);
	if (i >= 0){
		keyHandleList[i] = h;
		return true;
	}else{
		return false;
	}
}

//returns true if successful
bool keyUnRegisterHandle(keyPressHandle_t h){
	int i = findHandle(h);
	if (i<0){
		return false;
	}
	while(i>=0){
		keyHandleList[i] = NULL;
		i = findHandle(h);
	}
	return true;
}

void fireEvent(key_event_t event, key_id_t keyID){
	int i;
	for(i=0;i<HANDLELIST_SIZE;i++){
		if (keyHandleList[i] != NULL){
			keyHandleList[i](event,keyID);
		}
	}
}

void taskKey( void *pvParameters )
{
	int i;
	for( ;; )
	{
		i++;
		int keyID;

		for(keyID = kid_none+1;keyID<kid_KEYCOUNT;keyID++){
			if (boardTestKey(keyID)){
				if (keyList[keyID] == 0){
					fireEvent(ke_pressShort,keyID);
				}

				if(keyList[keyID] < KEYPRESS_LONG_THRESHOLD+1)
					keyList[keyID]++;

				if (keyList[keyID] == KEYPRESS_LONG_THRESHOLD){
					fireEvent(ke_pressLong,keyID);
				}

			}else{
				if (keyList[keyID]){
					fireEvent(ke_release,keyID);
				}
				keyList[keyID]=0;
			}
		}
		vTaskDelay(( 50 / portTICK_RATE_MS ));

	}
}
