#include "pal/pal_clock.h"
#include "pal/pal_gpio.h"
#include "pal/pal_uart.h"
#include "modules/blink/blink.h"
#include <stdio.h>

int _write(int fd, char *ptr, int len){ (void)fd; pal_uart_write((uint8_t*)ptr,(uint32_t)len); return len; } // printf()

int main(void)
{
  pal_clock_cfg_t clk = { .src = PAL_CLK_SRC_HSE_8MHz, .sysclk_hz=72000000, .ahb_hz=72000000, .apb1_hz=36000000, .apb2_hz=72000000 };
  pal_clock_init(&clk);

  pal_uart_cfg_t u = { .instance = USART1, .baud = 115200 };
  pal_uart_init(&u);

  pal_gpio_cfg_t pins[] = {
    {GPIOC, (1U<<13), PAL_GPIO_OUTPUT_PP, PAL_GPIO_NOPULL, PAL_GPIO_2MHz}
  };
  pal_gpio_init(pins, 1);

  printf("Hello from PAL @ %lu Hz\r\n", pal_clock_get_sysclk());

  while (1) { blink_tick(); }   // simple module loop
}
