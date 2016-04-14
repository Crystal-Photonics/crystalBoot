
#ifndef MCU2QT
#define MCU2QT
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#include <stdint.h>

typedef enum {rpcKeyStatus_none,rpcKeyStatus_pressed,rpcKeyStatus_pressedLong, rpcKeyStatus_released} rpcKeyStatus_t;


#pragma RPC prefix RPC_TRANSMISSION_

#pragma RPC noanswer qtUpdateMCUADCValues
    void qtUpdateMCUADCValues(int16_t tempMCU, int16_t adc_vcc_mv, int16_t adc1, int16_t adc2);

    void qtKeyPressed(rpcKeyStatus_t keyStatus);
#ifdef __cplusplus
}


#endif /* __cplusplus */
#endif
