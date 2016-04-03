#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#pragma RPC prefix RPC_TRANSMISSION_
	
#pragma RPC noanswer qtUpdateRealTemperature
	void qtUpdateRealTemperature(uint16_t data);
#ifdef __cplusplus
}
#endif /* __cplusplus */
