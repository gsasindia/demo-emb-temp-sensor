#include "main.h"
#include "temp_sensor.h"
#include <stdio.h>
#include <string.h>

ADC_HandleTypeDef hadc1;
UART_HandleTypeDef hlpuart1;

static void gpio_init(void);
static void lpuart1_init(void);
static void send_reading(float temp_c, uint32_t uptime_ms);

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    gpio_init();
    lpuart1_init();
    temp_sensor_init(&hadc1);

    float temp;
    uint32_t tick;

    while (1) {
        temp = temp_sensor_read(&hadc1);
        tick = HAL_GetTick();

        send_reading(temp, tick);

        /* Blink LD1 to indicate a reading was taken */
        HAL_GPIO_WritePin(LD1_PORT, LD1_PIN, GPIO_PIN_SET);
        HAL_Delay(200);
        HAL_GPIO_WritePin(LD1_PORT, LD1_PIN, GPIO_PIN_RESET);

        HAL_Delay(READING_INTERVAL_MS - 200);
    }
}

static void send_reading(float temp_c, uint32_t uptime_ms)
{
    char buf[64];
    int len = snprintf(buf, sizeof(buf),
                       "{\"temp_c\":%.1f,\"ts\":%lu}\r\n",
                       temp_c, (unsigned long)uptime_ms);

    HAL_UART_Transmit(&hlpuart1, (uint8_t *)buf, (uint16_t)len, HAL_MAX_DELAY);
}

static void gpio_init(void)
{
    GPIO_InitTypeDef gpio = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    /* LD1 — PC7 push-pull output */
    HAL_GPIO_WritePin(LD1_PORT, LD1_PIN, GPIO_PIN_RESET);
    gpio.Pin = LD1_PIN;
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LD1_PORT, &gpio);
}

static void lpuart1_init(void)
{
    __HAL_RCC_LPUART1_CLK_ENABLE();

    GPIO_InitTypeDef gpio = {0};
    gpio.Pin = VCP_TX_PIN | VCP_RX_PIN;
    gpio.Mode = GPIO_MODE_AF_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    gpio.Alternate = GPIO_AF8_LPUART1;
    HAL_GPIO_Init(VCP_TX_PORT, &gpio);

    hlpuart1.Instance = LPUART1;
    hlpuart1.Init.BaudRate = 115200;
    hlpuart1.Init.WordLength = UART_WORDLENGTH_8B;
    hlpuart1.Init.StopBits = UART_STOPBITS_1;
    hlpuart1.Init.Parity = UART_PARITY_NONE;
    hlpuart1.Init.Mode = UART_MODE_TX;
    hlpuart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;

    HAL_UART_Init(&hlpuart1);
}

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef osc = {0};
    RCC_ClkInitTypeDef clk = {0};

    /* Enable PWR clock and set voltage scaling */
    __HAL_RCC_PWR_CLK_ENABLE();
    HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

    /* MSI at 4 MHz as default clock */
    osc.OscillatorType = RCC_OSCILLATORTYPE_MSI;
    osc.MSIState = RCC_MSI_ON;
    osc.MSIClockRange = RCC_MSIRANGE_4;  /* 4 MHz */
    osc.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
    osc.PLL.PLLState = RCC_PLL_NONE;
    HAL_RCC_OscConfig(&osc);

    clk.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                  | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2
                  | RCC_CLOCKTYPE_PCLK3;
    clk.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
    clk.AHBCLKDivider = RCC_SYSCLK_DIV1;
    clk.APB1CLKDivider = RCC_HCLK_DIV1;
    clk.APB2CLKDivider = RCC_HCLK_DIV1;
    clk.APB3CLKDivider = RCC_HCLK_DIV1;
    HAL_RCC_ClockConfig(&clk, FLASH_LATENCY_0);
}

void Error_Handler(void)
{
    __disable_irq();
    while (1) {
        /* Stay here */
    }
}
