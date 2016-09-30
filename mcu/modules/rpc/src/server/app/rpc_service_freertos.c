/*
 * rpc_service_protothreads.c
 *
 *  Created on: 15.11.2015
 *      Author: arne
 */

#include"../../../include/rpc_transmission/server/generated_general/RPC_types.h"
#include"../../../include/rpc_transmission/server/generated_general/RPC_TRANSMISSION_network.h"
#include "errorlogger/generic_eeprom_errorlogger.h"
#include "channel_codec/channel_codec.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "main.h"

static const uint16_t MAX_LOCKTIME_ms = 10*1000;

SemaphoreHandle_t  rpc_transmission_mutexes[RPC_MUTEX_COUNT];





/* Initializes all rpc mutexes. */
void RPC_TRANSMISSION_mutex_init(void){
	for(int i = 0; i<RPC_MUTEX_COUNT;i++){
		rpc_transmission_mutexes[i] = xSemaphoreCreateBinary();
		RPC_TRANSMISSION_mutex_unlock(i);
	}
}

/* Locks the mutex. If it is already locked it yields until it can lock the mutex. */
void RPC_TRANSMISSION_mutex_lock(RPC_mutex_id mutex_id){
	//printf("lock %d\n",mutex_id);
	if( xSemaphoreTake( rpc_transmission_mutexes[mutex_id], MAX_LOCKTIME_ms ) == pdTRUE ){
			//return 1;
	}else{
		GEN_ASSERT(0,errlog_E_MUTEX_mutex_could_not_be_locked_within_timeout,"rpc mutex %d, could not be locked within timeout of %d ms\n", mutex_id,MAX_LOCKTIME_ms);
		//return 0;
	}
}

/* Unlocks the mutex. The mutex is locked when the function is called. */
void RPC_TRANSMISSION_mutex_unlock(RPC_mutex_id mutex_id){
	//printf("unlock %d\n",mutex_id);
	xSemaphoreGive( rpc_transmission_mutexes[mutex_id]);
}

char RPC_TRANSMISSION_mutex_lock_timeout(RPC_mutex_id mutex_id){
	const int timeout_ms = 1000;
	//printf("lock timed %d\n",mutex_id);
	if( xSemaphoreTake( rpc_transmission_mutexes[mutex_id], timeout_ms / portTICK_RATE_MS ) == pdTRUE ){

			return 1;
		}else{
			return 0;
		}

}

/*  This function is called when a new message starts. {size} is the number of
		    bytes the message will require. In the implementation you can allocate  a
		    buffer or write a preamble. The implementation can be empty if you do not
		    need to do that. */
void RPC_TRANSMISSION_message_start(size_t size){
	channel_start_message_from_RPC(&cc_instances[channel_codec_comport_transmission],size);
}

/* Pushes a byte to be sent via network. You should put all the pushed bytes
		   into a buffer and send the buffer when RPC_TRANSMISSION_message_commit is called. If you run
		   out of buffer space you can send multiple partial messages as long as the
		   other side puts them back together. */
void RPC_TRANSMISSION_message_push_byte(unsigned char byte){
	channel_push_byte_from_RPC(&cc_instances[channel_codec_comport_transmission],byte);
}

/* This function is called when a complete message has been pushed using
		   RPC_TRANSMISSION_message_push_byte. Now is a good time to send the buffer over the network,
		   even if the buffer is not full yet. You may also want to free the buffer that
		   you may have allocated in the RPC_TRANSMISSION_message_start function.
		   RPC_TRANSMISSION_message_commit should return RPC_TRANSMISSION_SUCCESS if the buffer has been successfully
		   sent and RPC_TRANSMISSION_FAILURE otherwise. */
RPC_RESULT RPC_TRANSMISSION_message_commit(void){
	return channel_commit_from_RPC(&cc_instances[channel_codec_comport_transmission]);
}



RPC_SIZE_RESULT RPC_CHANNEL_CODEC_get_request_size(channel_codec_instance_t *instance, const void *buffer, size_t size_bytes){
	if (instance->aux.port == channel_codec_comport_transmission){
		return RPC_TRANSMISSION_get_request_size(buffer, size_bytes);
	}else{
		RPC_SIZE_RESULT result;
		result.size = 0;
		result.result = RPC_FAILURE;
		assert(0);
		return result;
	}
}

RPC_SIZE_RESULT RPC_CHANNEL_CODEC_get_answer_length(channel_codec_instance_t *instance, const void *buffer, size_t size_bytes){
	if (instance->aux.port == channel_codec_comport_transmission){
		return RPC_TRANSMISSION_get_answer_length(buffer, size_bytes);
	}else{
		RPC_SIZE_RESULT result;
		result.size = 0;
		result.result = RPC_FAILURE;
		assert(0);
		return result;
	}
}

void RPC_CHANNEL_CODEC_parse_request(channel_codec_instance_t *instance, const void *buffer, size_t size_bytes){
	if (instance->aux.port == channel_codec_comport_transmission){
		RPC_TRANSMISSION_parse_request(buffer, size_bytes);
	}else{
		assert(0);
	}
}

void RPC_CHANNEL_CODEC_parse_answer(channel_codec_instance_t *instance, const void *buffer, size_t size_bytes){
	if (instance->aux.port == channel_codec_comport_transmission){
		RPC_TRANSMISSION_parse_answer(buffer, size_bytes);
	}else{
		assert(0);
	}
}

void RPC_CHANNEL_CODEC_parser_init(channel_codec_instance_t *instance){
	if (instance->aux.port == channel_codec_comport_transmission){
		RPC_TRANSMISSION_Parser_init();
	}else{
		assert(0);
	}
}

