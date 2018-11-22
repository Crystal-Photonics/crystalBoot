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

#include "rpc_receiver.h"

//#include "board.h"
#include "channel_codec/channel_codec.h"

#include "channel_codec/phylayer.h"
#include "errorlogger/generic_eeprom_errorlogger.h"

#include "main.h"
#include "port_serial.h"

#define CHANNEL_CODEC_TX_BUFFER_SIZE 256
#define CHANNEL_CODEC_RX_BUFFER_SIZE 256

channel_codec_instance_t cc_instances[channel_codec_comport_COUNT];

static char cc_rxBuffers[channel_codec_comport_COUNT][CHANNEL_CODEC_RX_BUFFER_SIZE];
static char cc_txBuffers[channel_codec_comport_COUNT][CHANNEL_CODEC_TX_BUFFER_SIZE];

void ChannelCodec_errorHandler(channel_codec_instance_t *instance, channelCodecErrorNum_t ErrNum) {
    (void)ErrNum;
    (void)instance;
}

RPC_RESULT phyPushDataBuffer(channel_codec_instance_t *instance, const char *buffer, size_t length) {
    if (instance->aux.port == channel_codec_comport_transmission) {
        for (size_t i = 0; i < length; i++) {
            portSerialPutChar(buffer[i]);
        }
    }
    return RPC_SUCCESS;
}

void rpc_receiver_init() {
    RPC_TRANSMISSION_mutex_init();

    cc_instances[channel_codec_comport_transmission].aux.port = channel_codec_comport_transmission;

    channel_init_instance(&cc_instances[channel_codec_comport_transmission], cc_rxBuffers[channel_codec_comport_transmission],
                          CHANNEL_CODEC_RX_BUFFER_SIZE, cc_txBuffers[channel_codec_comport_transmission], CHANNEL_CODEC_TX_BUFFER_SIZE);
}

bool rpc_receive() {
    uint8_t inByte;
    if (portSerialGetChar(&inByte) == true) {
        channel_push_byte_to_RPC(&cc_instances[channel_codec_comport_transmission], inByte);
        return true;
    } else {
        return false;
    }
}
