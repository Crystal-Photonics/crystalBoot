/*
 * port_serial.h
 *
 *  Created on: 17.10.2016
 *      Author: ak
 */

#ifndef MCU_SRC_PORT_STM32_L1_INCLUDE_PORT_SERIAL_H_
#define MCU_SRC_PORT_STM32_L1_INCLUDE_PORT_SERIAL_H_

#include <stdint.h>


void SerialPutChar(uint8_t c);
void SerialPutString(char *s);

#endif /* MCU_SRC_PORT_STM32_L1_INCLUDE_PORT_SERIAL_H_ */
