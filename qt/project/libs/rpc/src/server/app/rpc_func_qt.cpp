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


extern "C" void qtUpdateRealTemperature(int16_t tempThermo, int16_t tempJunction, int16_t tempTarget,  int16_t tempError){
    float tc = tempThermo/4.0;
    float cj = tempJunction/16.0;
    qDebug() << "temperature" << QString("%1").arg(tc,8,'f',2) <<  QString("%1").arg(cj,8,'f',3) << tempTarget << tempError;
    serialWorkerForRPCFunc->wrapUpdateTemperature((float) tc );
}

