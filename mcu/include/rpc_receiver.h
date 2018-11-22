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

#ifndef MCU_INCLUDE_RPC_RECEIVER_H_
#define MCU_INCLUDE_RPC_RECEIVER_H_

#include "channel_codec/channel_codec_types.h"

extern channel_codec_instance_t cc_instances[channel_codec_comport_COUNT];

void rpc_receiver_init(void);

bool rpc_receive(void); // returns true if data received

#endif /* MCU_INCLUDE_RPC_RECEIVER_H_ */
