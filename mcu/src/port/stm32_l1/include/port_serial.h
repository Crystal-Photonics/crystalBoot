/*
 * port_serial.h
 *
 *  Created on: 17.10.2016
 *      Author: ak
 */

#ifndef MCU_SRC_PORT_STM32_L1_INCLUDE_PORT_SERIAL_H_
#define MCU_SRC_PORT_STM32_L1_INCLUDE_PORT_SERIAL_H_

#include <stdint.h>
#include <stdbool.h>


void portSerialInit(int baud);

void portSerialPutChar(uint8_t c);
void portSerialPutString(char *s);
bool portSerialGetChar(uint8_t *c);

#endif /* MCU_SRC_PORT_STM32_L1_INCLUDE_PORT_SERIAL_H_ */
