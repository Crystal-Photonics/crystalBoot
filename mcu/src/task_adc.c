/*
 *    Copyright (C) 2015  Crystal-Photonics GmbH
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <stdio.h>
#include <assert.h>
#include "task_adc.h"
#include "task.h"
#include "main.h"
#include "rpc_transmission/client/generated_app/RPC_TRANSMISSION_mcu2qt.h"

#include "channel_codec/channel_codec.h"

#define FACTORY_TSCALIB_MD_BASE         ((uint32_t)0x1FF80078)    /*!< Calibration Data Bytes base address for medium density devices*/
#define FACTORY_TSCALIB_MDP_BASE        ((uint32_t)0x1FF800F8)    /*!< Calibration Data Bytes base address for medium density plus devices*/
#define FACTORY_TSCALIB_MD_DATA         ((TSCALIB_TypeDef *) FACTORY_TSCALIB_MD_BASE)
#define FACTORY_TSCALIB_MDP_DATA        ((TSCALIB_TypeDef *) FACTORY_TSCALIB_MDP_BASE)
#define TEST_CALIB_DIFF           		(int32_t) 50  /* difference of hot-cold calib
                                               data to be considered as valid */

/*
STM32L151RET6:
VREFINT_CAL 	0x1FF8 00F8 - 0x1FF8 00F9 	Raw data acquired at temperature of 30 °C ±5 °C VDDA= 3 V ±10 mV
TSCAL1 			0x1FF8 00FA - 0x1FF8 00FB	TS ADC raw data acquired at temperature of 30 °C ±5 °C VDDA= 3 V ±10 mV
TSCAL2			0x1FF8 00FE - 0x1FF8 00FF	TS ADC raw data acquired at temperature of 110 °C ±5 °C VDDA= 3 V ±10 mV
*/
typedef struct
{
    uint16_t VREF;
    uint16_t TS_CAL_1; // low temperature calibration data
    uint16_t reserved;
    uint16_t TS_CAL_2; // high temperature calibration data
} TSCALIB_TypeDef;

static TSCALIB_TypeDef adcCalibData;
static SemaphoreHandle_t semaphoreADCReady;

uint32_t ubat_avgsum;
uint16_t adcValuesPlain[adsi_max-1];

const uint32_t adcCHANNELS[] = {ADC_CHANNEL_VREFINT,ADC_CHANNEL_TEMPSENSOR,ADC_CHAN_1,ADC_CHAN_2};

volatile adc_sequence_index_t adcSequenceIndex;



void getFactoryTSCalibData(TSCALIB_TypeDef* data)
{
	uint32_t deviceID;

	deviceID = HAL_GetDEVID();

	if (deviceID == 0x427) *data = *FACTORY_TSCALIB_MDP_DATA;
	else if (deviceID == 0x437) *data = *FACTORY_TSCALIB_MDP_DATA;
	else if (deviceID == 0x416) *data = *FACTORY_TSCALIB_MD_DATA;
	else{
		printf("Error loading ADC Calib Data\n");
		assert(0);
		while(1); // add error handler - device cannot be identified calibration data not loaded!
	}
}

ErrorStatus testFactoryCalibData(void)
{
  int32_t testdiff;
  ErrorStatus retval = ERROR;
  TSCALIB_TypeDef datatotest;
  getFactoryTSCalibData (&datatotest);
  testdiff = datatotest.TS_CAL_2 - datatotest.TS_CAL_1;
  if ( testdiff > TEST_CALIB_DIFF )    retval = SUCCESS;
  return retval;
}


/**
  * @brief  Configures the ADC1 channel5.
  * @param  None
  * @retval None
  */
void ADC_Config(void)
{




	ADC_ChannelConfTypeDef sConfig;

	semaphoreADCReady = xSemaphoreCreateBinary(  );

	if ( testFactoryCalibData() == SUCCESS )
		getFactoryTSCalibData(&adcCalibData);


	hadc.Instance = ADC1;
	hadc.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
	hadc.Init.Resolution = ADC_RESOLUTION_12B;
	hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc.Init.ScanConvMode = ADC_SCAN_ENABLE;
	hadc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	hadc.Init.LowPowerAutoWait = ADC_AUTOWAIT_UNTIL_DATA_READ;
	hadc.Init.LowPowerAutoPowerOff = ADC_AUTOPOWEROFF_IDLE_DELAY_PHASES;
	hadc.Init.ChannelsBank = ADC_CHANNELS_BANK_A;
	hadc.Init.ContinuousConvMode = DISABLE;
	hadc.Init.NbrOfConversion = adsi_max-1;
	hadc.Init.DiscontinuousConvMode = DISABLE;
	hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	hadc.Init.DMAContinuousRequests = DISABLE;
	HAL_ADC_Init(&hadc);

	for (int i = 0; i<adsi_max-1;i++ ){
		sConfig.Channel = adcCHANNELS[i];
		sConfig.Rank = i+1;
		sConfig.SamplingTime = ADC_SAMPLETIME_192CYCLES;
		HAL_ADC_ConfigChannel(&hadc, &sConfig);
	}



	adcSequenceIndex = 1;

	HAL_ADC_Start_IT(&hadc);


}


void taskADC(void *pvParameters) {

	const uint16_t SMOOTHVALUE = 16;
	ADC_Config();

	uint32_t adcValues_smoothed_SMOOTHVALUE[adsi_max-1];

	uint16_t adcValues_smoothed[adsi_max-1];

	for (int i=0;i<adsi_max-1;i++){
		adcValues_smoothed_SMOOTHVALUE[i] = 0;
	}
	for (;;) {

		if( xSemaphoreTake( semaphoreADCReady, 1000 / portTICK_RATE_MS ) == pdTRUE )
		{
			const int32_t VOLT_REFERENCE_mv = 3000;
			const int32_t ADC_MAX_VALUE_DIGIT = 4095;

			uint32_t vcc_mv = VOLT_REFERENCE_mv*adcCalibData.VREF;
			vcc_mv /= adcValuesPlain[adsi_ref-1];

			int32_t temperature_c = (int32_t)adcValuesPlain[adsi_temperature-1]*vcc_mv;
			temperature_c /= VOLT_REFERENCE_mv;

			temperature_c = (110-30)* (temperature_c - adcCalibData.TS_CAL_1);
			temperature_c /= (adcCalibData.TS_CAL_2  - adcCalibData.TS_CAL_1);
			temperature_c += 30;



			uint32_t adc1_mv = vcc_mv*adcValuesPlain[adsi_adc1-1];
			adc1_mv /= ADC_MAX_VALUE_DIGIT;

			uint32_t adc2_mv = vcc_mv*adcValuesPlain[adsi_adc2-1];
			adc2_mv /= ADC_MAX_VALUE_DIGIT;

			adcValues_smoothed_SMOOTHVALUE[adsi_temperature-1] += temperature_c;
			adcValues_smoothed_SMOOTHVALUE[adsi_ref-1] += vcc_mv;
			adcValues_smoothed_SMOOTHVALUE[adsi_adc1-1] += adc1_mv;
			adcValues_smoothed_SMOOTHVALUE[adsi_adc2-1] += adc2_mv;


			for (int i=0;i<adsi_max-1;i++){
				adcValues_smoothed[i] = adcValues_smoothed_SMOOTHVALUE[i]/SMOOTHVALUE;
				adcValues_smoothed_SMOOTHVALUE[i] -= adcValues_smoothed[i];

			}

			qtUpdateMCUADCValues(adcValues_smoothed[adsi_temperature-1], adcValues_smoothed[adsi_ref-1], adcValues_smoothed[adsi_adc1-1], adcValues_smoothed[adsi_adc2-1]);
			vTaskDelay(( 50 / portTICK_RATE_MS ));
			HAL_ADC_Start_IT(&hadc);
		}
	}
}

/**
  * @brief  This function handles ADC1 global interrupts requests.
  * @param  None
  * @retval None
  */
void ADC1_IRQHandler(void)
{
	  if(__HAL_ADC_GET_IT_SOURCE(&hadc, ADC_IT_EOC))
	  {
	    if(__HAL_ADC_GET_FLAG(&hadc, ADC_FLAG_EOC) ){
			portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;


			adcValuesPlain[adcSequenceIndex-1] = HAL_ADC_GetValue(&hadc);
			adcSequenceIndex++;
			if(adcSequenceIndex >= adsi_max){
				xSemaphoreGiveFromISR(semaphoreADCReady,&xHigherPriorityTaskWoken);
				adcSequenceIndex = 1;
			}
			__HAL_ADC_CLEAR_FLAG(&hadc, ADC_FLAG_STRT | ADC_FLAG_EOC);
			portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
	    }
	  }

}
