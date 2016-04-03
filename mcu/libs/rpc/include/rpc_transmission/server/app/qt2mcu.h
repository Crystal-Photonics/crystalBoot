#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#pragma RPC prefix RPC_TRANSMISSION_
	
#pragma RPC noanswer mcuSendDataNoAnswer
	void mcuSendDataNoAnswer(uint8_t data);
	
uint16_t mcuSetMCUTargetTemperature(uint16_t data);
#ifdef __cplusplus
}
#endif /* __cplusplus */
