/*
 * chip_init.h
 *
 *  Created on: 20.04.2016
 *      Author: ak
 */

#ifndef MCU_INCLUDE_CHIP_INIT_H_
#define MCU_INCLUDE_CHIP_INIT_H_

extern ADC_HandleTypeDef hadc;

extern DAC_HandleTypeDef hdac;

extern I2C_HandleTypeDef hi2c2;

extern RTC_HandleTypeDef hrtc;

extern SPI_HandleTypeDef hspi2;

extern TIM_HandleTypeDef htim4;

extern UART_HandleTypeDef huart1;

void SystemClock_Config(void );
void MX_RTC_Init(void);
void MX_ADC_Init(void);
#endif /* MCU_INCLUDE_CHIP_INIT_H_ */
