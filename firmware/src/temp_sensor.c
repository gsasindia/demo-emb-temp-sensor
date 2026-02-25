#include "temp_sensor.h"

/* Factory calibration addresses for STM32U575 */
#define TS_CAL1_ADDR  ((uint16_t *)0x0BFA0710)  /* @30°C, VDDA=3.0V */
#define TS_CAL2_ADDR  ((uint16_t *)0x0BFA0742)  /* @130°C, VDDA=3.0V */
#define TS_CAL1_TEMP  30.0f
#define TS_CAL2_TEMP  130.0f
#define VDDA_APPLI    3.3f
#define VDDA_CAL      3.0f
#define ADC_MAX       16383  /* 14-bit ADC */

void temp_sensor_init(ADC_HandleTypeDef *hadc)
{
    ADC_ChannelConfTypeDef sConfig = {0};

    hadc->Instance = ADC1;
    hadc->Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV4;
    hadc->Init.Resolution = ADC_RESOLUTION_14B;
    hadc->Init.ScanConvMode = ADC_SCAN_DISABLE;
    hadc->Init.ContinuousConvMode = DISABLE;
    hadc->Init.DiscontinuousConvMode = DISABLE;
    hadc->Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc->Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc->Init.NbrOfConversion = 1;
    hadc->Init.DMAContinuousRequests = DISABLE;
    hadc->Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    hadc->Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;

    HAL_ADC_Init(hadc);
    HAL_ADCEx_Calibration_Start(hadc, ADC_SINGLE_ENDED);

    sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_391CYCLES_5;
    sConfig.SingleDiff = ADC_SINGLE_ENDED;
    sConfig.OffsetNumber = ADC_OFFSET_NONE;

    HAL_ADC_ConfigChannel(hadc, &sConfig);
}

float temp_sensor_read(ADC_HandleTypeDef *hadc)
{
    uint16_t ts_cal1 = *TS_CAL1_ADDR;
    uint16_t ts_cal2 = *TS_CAL2_ADDR;
    uint32_t adc_raw;

    HAL_ADC_Start(hadc);
    HAL_ADC_PollForConversion(hadc, HAL_MAX_DELAY);
    adc_raw = HAL_ADC_GetValue(hadc);
    HAL_ADC_Stop(hadc);

    /* Compensate for VDDA difference between calibration and application */
    float ts_cal1_adj = (float)ts_cal1 * (VDDA_CAL / VDDA_APPLI);
    float ts_cal2_adj = (float)ts_cal2 * (VDDA_CAL / VDDA_APPLI);

    float temp = TS_CAL1_TEMP
               + ((float)adc_raw - ts_cal1_adj)
               * (TS_CAL2_TEMP - TS_CAL1_TEMP)
               / (ts_cal2_adj - ts_cal1_adj);

    return temp;
}
