#include"rpc_transmission/server/generated_general/RPC_TRANSMISSION_types.h"
#include "serial.h"
RPC_TRANSMISSION_RESULT phyPushDataBuffer(const char *buffer, size_t length){

	for (int i=0;i<length;i++){
		xSerialPutChar(buffer[i]);
	}
	return RPC_TRANSMISSION_SUCCESS;
}