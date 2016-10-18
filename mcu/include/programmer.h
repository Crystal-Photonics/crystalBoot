/*
 * programmer.h
 *
 *  Created on: 18.10.2016
 *      Author: ak
 */

#ifndef MCU_INCLUDE_PROGRAMMER_H_
#define MCU_INCLUDE_PROGRAMMER_H_
#include <stdint.h>
#include <stddef.h>

void programmerReset(void);
void programmerWriteBlock(uint8_t *data, size_t size);
void programmerReadBlock(uint8_t *data, size_t size);



#endif /* MCU_INCLUDE_PROGRAMMER_H_ */
