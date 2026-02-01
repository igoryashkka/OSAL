#include "Platform_GPIO/Platform_gpio.h"

/* Minimal GPIO implementation for STM8 builds with SDCC.
 * Full SPL headers are compiler-specific and not currently compatible
 * with SDCC in-tree. These stubs allow the code to build; replace
 * with real register-level access when targeting actual hardware.
 */
static volatile int _led_state = 0;

void GPIO_InitLed(void)
{
    /* No hardware init for now */
    _led_state = 0;
}

void GPIO_ToggleLed(void)
{
    _led_state = !_led_state;
}
