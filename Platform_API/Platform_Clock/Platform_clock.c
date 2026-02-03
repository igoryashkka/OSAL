#include "platform_clock.h"

#ifndef BUILD_FOR_STM8
__attribute__((weak)) void RCC_InitDefault(void) { /* no-op */ }
#endif
