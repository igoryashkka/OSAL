#pragma once
#include <stdint.h>
#include "stm32f1xx.h" // for GPIO_TypeDef if you want HAL types; or wrap

typedef enum { PAL_GPIO_INPUT=0, PAL_GPIO_OUTPUT_PP, PAL_GPIO_OUTPUT_OD, PAL_GPIO_AF_PP, PAL_GPIO_AF_OD, PAL_GPIO_ANALOG } pal_gpio_mode_t;
typedef enum { PAL_GPIO_NOPULL=0, PAL_GPIO_PULLUP, PAL_GPIO_PULLDOWN } pal_gpio_pull_t;
typedef enum { PAL_GPIO_2MHz=2, PAL_GPIO_10MHz=1, PAL_GPIO_50MHz=3 } pal_gpio_speed_t;

typedef struct {
  GPIO_TypeDef *port;
  uint16_t pin_mask;          // e.g. (1U<<9)
  pal_gpio_mode_t mode;
  pal_gpio_pull_t pull;
  pal_gpio_speed_t speed;
} pal_gpio_cfg_t;

void pal_gpio_init(const pal_gpio_cfg_t *cfg, uint32_t count);
void pal_gpio_write(GPIO_TypeDef *port, uint16_t pin_mask, int level);
int  pal_gpio_read(GPIO_TypeDef *port, uint16_t pin_mask);

