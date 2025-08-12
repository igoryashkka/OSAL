#include "pal/pal_uart.h"
#include "stm32f1xx_hal.h"

static UART_HandleTypeDef huart;

int pal_uart_init(const pal_uart_cfg_t *cfg)
{
  __HAL_RCC_USART1_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE(); // USART1 PA9/PA10

  GPIO_InitTypeDef g = {0};
  g.Pin   = GPIO_PIN_9;  g.Mode = GPIO_MODE_AF_PP; g.Speed = GPIO_SPEED_FREQ_HIGH; HAL_GPIO_Init(GPIOA, &g);
  g.Pin   = GPIO_PIN_10; g.Mode = GPIO_MODE_INPUT; g.Pull  = GPIO_NOPULL;         HAL_GPIO_Init(GPIOA, &g);

  huart.Instance        = (USART_TypeDef*)cfg->instance;  // pass USART1 from app
  huart.Init.BaudRate   = cfg->baud;
  huart.Init.WordLength = UART_WORDLENGTH_8B;
  huart.Init.StopBits   = UART_STOPBITS_1;
  huart.Init.Parity     = UART_PARITY_NONE;
  huart.Init.Mode       = UART_MODE_TX_RX;
  huart.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
  huart.Init.OverSampling = UART_OVERSAMPLING_16;
  return (HAL_UART_Init(&huart)==HAL_OK)?0:-1;
}

int pal_uart_write(const uint8_t *buf, uint32_t len){ return (HAL_UART_Transmit(&huart,(uint8_t*)buf,len,1000)==HAL_OK)?(int)len:-1; }
int pal_uart_read(uint8_t *buf, uint32_t len, uint32_t t){ return (HAL_UART_Receive(&huart,buf,len,t)==HAL_OK)?(int)len:-1; }
