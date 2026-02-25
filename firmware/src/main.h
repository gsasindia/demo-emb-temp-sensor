#ifndef MAIN_H
#define MAIN_H

#include "stm32u5xx_hal.h"

/* LED — LD1 green on PC7 */
#define LD1_PIN   GPIO_PIN_7
#define LD1_PORT  GPIOC

/* LPUART1 pins (ST-Link VCP) */
#define VCP_TX_PIN   GPIO_PIN_9
#define VCP_TX_PORT  GPIOA
#define VCP_RX_PIN   GPIO_PIN_10
#define VCP_RX_PORT  GPIOA

/* Reading interval in milliseconds (2 minutes) */
#define READING_INTERVAL_MS  120000

/* Peripheral handles */
extern ADC_HandleTypeDef hadc1;
extern UART_HandleTypeDef hlpuart1;

/* Function prototypes */
void SystemClock_Config(void);
void Error_Handler(void);

#endif /* MAIN_H */
