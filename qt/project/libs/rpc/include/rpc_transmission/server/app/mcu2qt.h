

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#include <stdint.h>




#pragma RPC prefix RPC_TRANSMISSION_

#pragma RPC noanswer qtUpdateRealTemperature
    void qtUpdateRealTemperature(int16_t tempThermo, int16_t tempJunction, int16_t tempTarget,  int16_t tempError);
#ifdef __cplusplus
}
#endif /* __cplusplus */
