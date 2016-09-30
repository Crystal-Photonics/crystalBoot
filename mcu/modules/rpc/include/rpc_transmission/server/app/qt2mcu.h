#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#pragma RPC prefix RPC_TRANSMISSION_
	
typedef enum {rpcLEDStatus_none,rpcLEDStatus_off,rpcLEDStatus_on} rpcLEDStatus_t;

#pragma RPC noanswer mcuSendDataNoAnswer
	void mcuSendDataNoAnswer(uint8_t data);
	
uint16_t mcuSetLEDStatus(rpcLEDStatus_t ledStatus);

#ifdef __cplusplus
}
#endif /* __cplusplus */
