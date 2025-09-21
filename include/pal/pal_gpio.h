#pragma once
//#include <stdint.h>
#ifndef STM32_MODEL
#error "STM32 MODEL NOT DEFINED"
#endif

if STM32_MODEL == f1xx
    
    #include "stm32f1xx_hal_gpio.h"
    #include "stm32f1xx_hal_rcc.h"

#elif STM32_MODEL == g0xx

    #include "stm32g0xx_hal_gpio.h"
    #include "stm32g0xx_hal_rcc.h"

#else
    #error "Unknown STM32_MODEL"
#endif

#include "stm32f1xx_hal.h"

static void delay_cycles(volatile uint32_t n) { while (n--) __asm__ volatile("nop"); }

#define pal_delay(ms) delay_cycles(ms)

void pal_gpio_init_led(void);
void pal_gpio_toggle_led(void);
