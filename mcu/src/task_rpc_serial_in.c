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

#define CHANNEL_CODEC_TX_BUFFER_SIZE 64
#define CHANNEL_CODEC_RX_BUFFER_SIZE 64


static char cc_rxBuffers[channel_codec_comport_COUNT][CHANNEL_CODEC_RX_BUFFER_SIZE];
static char cc_txBuffers[channel_codec_comport_COUNT][CHANNEL_CODEC_TX_BUFFER_SIZE];

void ChannelCodec_errorHandler(channel_codec_instance_t *instance,  channelCodecErrorNum_t ErrNum){
	(void)ErrNum;
	(void)instance;

}



RPC_RESULT phyPushDataBuffer(channel_codec_instance_t *instance,  const char *buffer, size_t length){
	if (instance->aux.port == channel_codec_comport_transmission){
		vSerialPutString(serCOM_DBG, buffer,  length);
	}
	return RPC_SUCCESS;
}

void taskRPCSerialIn(void *pvParameters) {

	signed char  inByte;


	cc_instances[channel_codec_comport_transmission].aux.port = channel_codec_comport_transmission;

	channel_init_instance(&cc_instances[channel_codec_comport_transmission],
									 cc_rxBuffers[channel_codec_comport_transmission],CHANNEL_CODEC_RX_BUFFER_SIZE,
									 cc_txBuffers[channel_codec_comport_transmission],CHANNEL_CODEC_TX_BUFFER_SIZE);

    for (int i = 0;i < taskHandleID_count; i++){
    	if (i != taskHandleID_RPCSerialIn)
    		vTaskResume(taskHandles[i]);
    }


    vTaskDelay(( 50 / portTICK_RATE_MS ));
	for (;;) {
		if (xSerialGetChar( serCOM_DBG, &inByte, 100 / portTICK_RATE_MS ) == pdTRUE){
			channel_push_byte_to_RPC(&cc_instances[channel_codec_comport_transmission],inByte);
		}
	}
}
