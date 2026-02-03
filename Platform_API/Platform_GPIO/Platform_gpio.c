#include "platform_gpio.h"

/* Weak stubs: if a platform doesn't provide an implementation, you will
 * still link, but you'll get no-op behavior (or you can assert in debug).
 */
#ifndef BUILD_FOR_STM8
__attribute__((weak)) void GPIO_InitLed(void)  { /* no-op */ }
__attribute__((weak)) void GPIO_ToggleLed(void){ /* no-op */ }
#endif
