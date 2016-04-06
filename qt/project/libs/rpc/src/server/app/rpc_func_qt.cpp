/*
 * rpc_func_arduino.c
 *
 *  Created on: 15.11.2015
 *      Author: arne
 */
#include <QDebug>

#include "../../../include/rpc_transmission/server/app/mcu2qt.h"
#include "serialworker.h"


extern "C" void qtUpdateRealTemperature(uint16_t data){
    qDebug() << "temperature" << data/4;
    serialWorkerForRPCFunc->wrapUpdateTemperature((float) data );
}

