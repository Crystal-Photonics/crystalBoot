/*
 * channel_codec_config.h
 *
 *  Created on: 02.08.2016
 *      Author: ak
 */

#ifndef TESTS_INCLUDE_CHANNEL_CODEC_CHANNEL_CODEC_CONFIG_H_
#define TESTS_INCLUDE_CHANNEL_CODEC_CHANNEL_CODEC_CONFIG_H_


#include "rpc_transmission/server/generated_general/RPC_TRANSMISSION_network.h"
#include "rpc_transmission/server/generated_general/RPC_TRANSMISSION_parser.h"
#include "rpc_transmission/client/generated_general/RPC_types.h"
//#include "serialworker.h"

struct SerialThread;


    typedef enum{
        channel_codec_comport_transmission,
        channel_codec_comport_COUNT
    } channel_codec_conf_comport_index_t;

    typedef struct{
        channel_codec_conf_comport_index_t portindex;
        struct SerialThread* serialthread;
    }channel_codec_conf_auxdata_t;




#endif /* TESTS_INCLUDE_CHANNEL_CODEC_CHANNEL_CODEC_CONFIG_H_ */
