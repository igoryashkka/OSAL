#include "pal/pal_gpio.h"
#include "stm32f1xx_hal.h"

static uint32_t to_hal_speed(pal_gpio_speed_t s){ return (s==PAL_GPIO_2MHz)?GPIO_SPEED_FREQ_LOW:(s==PAL_GPIO_10MHz)?GPIO_SPEED_FREQ_MEDIUM:GPIO_SPEED_FREQ_HIGH; }

void pal_gpio_init(const pal_gpio_cfg_t *cfg, uint32_t count)
{
  __HAL_RCC_AFIO_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE(); __HAL_RCC_GPIOB_CLK_ENABLE(); __HAL_RCC_GPIOC_CLK_ENABLE(); // enable as needed

  for (uint32_t i=0;i<count;i++) {
    GPIO_InitTypeDef g = {0};
    g.Pin   = cfg[i].pin_mask;
    g.Pull  = (cfg[i].pull==PAL_GPIO_PULLUP)?GPIO_PULLUP:(cfg[i].pull==PAL_GPIO_PULLDOWN)?GPIO_PULLDOWN:GPIO_NOPULL;
    g.Speed = to_hal_speed(cfg[i].speed);
    switch (cfg[i].mode) {
      case PAL_GPIO_INPUT:     g.Mode = GPIO_MODE_INPUT; break;
      case PAL_GPIO_OUTPUT_PP: g.Mode = GPIO_MODE_OUTPUT_PP; break;
      case PAL_GPIO_OUTPUT_OD: g.Mode = GPIO_MODE_OUTPUT_OD; break;
      case PAL_GPIO_AF_PP:     g.Mode = GPIO_MODE_AF_PP; break;
      case PAL_GPIO_AF_OD:     g.Mode = GPIO_MODE_AF_OD; break;
      case PAL_GPIO_ANALOG:    g.Mode = GPIO_MODE_ANALOG; break;
    }
    HAL_GPIO_Init(cfg[i].port, &g);
  }
}

void pal_gpio_write(GPIO_TypeDef *port, uint16_t mask, int level){ HAL_GPIO_WritePin(port, mask, level?GPIO_PIN_SET:GPIO_PIN_RESET); }
int  pal_gpio_read(GPIO_TypeDef *port, uint16_t mask){ return HAL_GPIO_ReadPin(port, mask)==GPIO_PIN_SET; }
