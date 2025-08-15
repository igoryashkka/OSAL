#include "stm32f1xx_hal.h"
#include "pal_gpio.h"
#include "blink.h"

void blink_once(void) {
    pal_gpio_toggle_led();
    HAL_Delay(50);
    pal_gpio_toggle_led();
}
