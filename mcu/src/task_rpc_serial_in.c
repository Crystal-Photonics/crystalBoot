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
#include "task_rpc_serial_in.h"

#include "channel_codec/channel_codec.h"

#include "channel_codec/phylayer.h"
#include "errorlogger/generic_eeprom_errorlogger.h"

#include "main.h"
#include "serial.h"

void ChannelCodec_errorHandler(channelCodecErrorNum_t ErrNum){
	(void)ErrNum;

}


RPC_TRANSMISSION_RESULT phyPushDataBuffer(const char *buffer, size_t length){
	vSerialPutString(serCOM_DBG, buffer,  length);
	return RPC_TRANSMISSION_SUCCESS;
}

void taskRPCSerialIn(void *pvParameters) {

	signed char  inByte;
	channel_init();



    for (int i = 0;i < taskHandleID_count; i++){
    	if (i != taskHandleID_RPCSerialIn)
    		vTaskResume(taskHandles[i]);
    }


    vTaskDelay(( 50 / portTICK_RATE_MS ));
	for (;;) {
		if (xSerialGetChar( serCOM_DBG, &inByte, 100 / portTICK_RATE_MS ) == pdTRUE){
			channel_push_byte_to_RPC(inByte);
		}
	}
}
