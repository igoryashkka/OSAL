#include "stm32f1xx_hal.h"
//#include "config/board.h"

void GPIO_InitLed(void)
{
    //LED_GPIO_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = 13;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    HAL_GPIO_WritePin(GPIOC, 13, GPIO_PIN_RESET);
}

void GPIO_ToggleLed(void)
{
    HAL_GPIO_TogglePin(GPIOC, 13);
}
