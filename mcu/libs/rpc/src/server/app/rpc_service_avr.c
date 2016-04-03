/*
 * rpc_service_protothreads.c
 *
 *  Created on: 15.11.2015
 *      Author: arne
 */

#include"../../../include/rpc_transmission/server/generated_general/RPC_TRANSMISSION_types.h"
#include"../../../include/rpc_transmission/server/generated_general/RPC_TRANSMISSION_network.h"
#include"../../../../../src/serial.h"
#include "channel_codec/channel_codec.h"

//struct pt *;

/* Initializes all rpc mutexes. */
void RPC_TRANSMISSION_mutex_init(void){

}

/* Locks the mutex. If it is already locked it yields until it can lock the mutex. */
void RPC_TRANSMISSION_mutex_lock(RPC_TRANSMISSION_mutex_id mutex_id){

}

/* Unlocks the mutex. The mutex is locked when the function is called. */
void RPC_TRANSMISSION_mutex_unlock(RPC_TRANSMISSION_mutex_id mutex_id){

}

char RPC_TRANSMISSION_mutex_lock_timeout(RPC_TRANSMISSION_mutex_id mutex_id){
	return 0;
}

/*  This function is called when a new message starts. {size} is the number of
		    bytes the message will require. In the implementation you can allocate  a
		    buffer or write a preamble. The implementation can be empty if you do not
		    need to do that. */
void RPC_TRANSMISSION_message_start(size_t size){
	channel_start_message_from_RPC(size);
}

/* Pushes a byte to be sent via network. You should put all the pushed bytes
		   into a buffer and send the buffer when RPC_TRANSMISSION_message_commit is called. If you run
		   out of buffer space you can send multiple partial messages as long as the
		   other side puts them back together. */
void RPC_TRANSMISSION_message_push_byte(unsigned char byte){
	channel_push_byte_from_RPC(byte);
}

/* This function is called when a complete message has been pushed using
		   RPC_TRANSMISSION_message_push_byte. Now is a good time to send the buffer over the network,
		   even if the buffer is not full yet. You may also want to free the buffer that
		   you may have allocated in the RPC_TRANSMISSION_message_start function.
		   RPC_TRANSMISSION_message_commit should return RPC_TRANSMISSION_SUCCESS if the buffer has been successfully
		   sent and RPC_TRANSMISSION_FAILURE otherwise. */
RPC_TRANSMISSION_RESULT RPC_TRANSMISSION_message_commit(void){
	return channel_commit_from_RPC();
}



