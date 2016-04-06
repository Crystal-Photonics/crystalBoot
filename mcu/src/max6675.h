/*
 * max6675.h
 *
 *  Created on: 15.07.2013
 *      Author: arne
 */

#ifndef MAX6675_H_
#define MAX6675_H_

typedef struct{
int16_t temperature_thermocuouple;
int16_t temperature_coldjunction;

unsigned int fault:1;
unsigned int scvFault:1;
unsigned int scgFault:1;
unsigned int ocFault:1;

}max33185result_t;

int8_t max6675Read(max33185result_t *temp);
void max6675Init(void);

#endif /* MAX6675_H_ */
