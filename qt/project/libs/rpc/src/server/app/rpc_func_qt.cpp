/*
 * rpc_func_arduino.c
 *
 *  Created on: 15.11.2015
 *      Author: arne
 */
#include <QDebug>
#include <QString>
#include "../../../include/rpc_transmission/server/app/mcu2qt.h"
#include "serialworker.h"


extern "C" void qtUpdateMCUADCValues(int16_t tempMCU, int16_t adc_vcc_mv, int16_t adc1, int16_t adc2){
    (void)tempMCU;
    (void)adc1;
    (void)adc_vcc_mv;

    (void)adc2;
    //qDebug() << "adc" << tempMCU <<  adc1 << adc2;
    serialWorkerForRPCFunc->wrapUpdateADC((float) adc1 );
}

extern "C" void qtKeyPressed(rpcKeyStatus_t keyStatus){
    QString statusStr;
    switch (keyStatus){
    case rpcKeyStatus_none:
        statusStr = "none";
        break;
    case rpcKeyStatus_pressed:
        statusStr = "pressed";
        break;
    case rpcKeyStatus_pressedLong:
        statusStr = "pressed long";
        break;
    case rpcKeyStatus_released:
        statusStr = "released";
        break;
    }

    qDebug() << "key pressed" << statusStr;
    serialWorkerForRPCFunc->wrapUpdateKeyState(keyStatus);
}


