#include "stm32f1xx_hal.h"
#include "config/board.h"
#include <string.h>

static UART_HandleTypeDef huart;

void pal_uart_init(uint32_t baud)
{
    UARTx_CLK_ENABLE();
    UARTx_GPIO_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = UARTx_TX_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(UARTx_GPIO_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = UARTx_RX_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(UARTx_GPIO_PORT, &GPIO_InitStruct);

    huart.Instance = UARTx;
    huart.Init.BaudRate = baud;
    huart.Init.WordLength = UART_WORDLENGTH_8B;
    huart.Init.StopBits = UART_STOPBITS_1;
    huart.Init.Parity = UART_PARITY_NONE;
    huart.Init.Mode = UART_MODE_TX_RX;
    huart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart.Init.OverSampling = UART_OVERSAMPLING_16;
    HAL_UART_Init(&huart);
}

int pal_uart_write(const uint8_t* data, int len)
{
    if (!data || len <= 0) return 0;
    if (HAL_UART_Transmit(&huart, (uint8_t*)data, (uint16_t)len, 100) == HAL_OK)
        return len;
    return 0;
}
