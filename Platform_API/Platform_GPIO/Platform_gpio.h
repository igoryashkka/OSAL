#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Platform GPIO API (implemented by platform; weak stubs live in Platform_API).
 *
 * Minimal OSAL-style wrapper. Keep this header MCU-agnostic.
 */

void GPIO_InitLed(void);
void GPIO_ToggleLed(void);

#ifdef __cplusplus
}
#endif
