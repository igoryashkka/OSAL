#pragma once
//#include <stdint.h>
#include "stm32f1xx_hal.h"

static void delay_cycles(volatile uint32_t n) { while (n--) __asm__ volatile("nop"); }

#define pal_delay(ms) delay_cycles(ms)

void pal_gpio_init_led(void);
void pal_gpio_toggle_led(void);
