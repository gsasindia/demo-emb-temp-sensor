#ifndef TEMP_SENSOR_H
#define TEMP_SENSOR_H

#include "stm32u5xx_hal.h"

/**
 * Initialize ADC1 for internal temperature sensor reading.
 * Must be called after HAL_Init() and SystemClock_Config().
 */
void temp_sensor_init(ADC_HandleTypeDef *hadc);

/**
 * Read the internal temperature sensor.
 * Uses factory calibration values (TS_CAL1 @ 30C, TS_CAL2 @ 130C).
 * Returns temperature in degrees Celsius.
 */
float temp_sensor_read(ADC_HandleTypeDef *hadc);

#endif /* TEMP_SENSOR_H */
