#ifndef LED_H_
#define LED_H_

#include "board.h"

#define TASK_LED_SPEED_MASK_LO 0x0001
#define TASK_LED_SPEED_MASK_HI 0x0000



typedef struct{
	uint8_t leds;
} t_task_data_led;

t_task_data_led led_data;
void task_led_ini(t_task_data_led* data); 
void task_led_trigger(t_task_data_led* data);
 
#endif /*LED_H_*/
