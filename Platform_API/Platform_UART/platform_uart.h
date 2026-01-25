#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Platform UART API - MCU-agnostic wrapper
 * Implemented by platform layer; weak stubs in Platform_API
 */

void Platform_UART_Init(uint32_t baud);
int  Platform_UART_Write(const uint8_t* data, int len);
int  Platform_UART_Read(uint8_t* buffer, int max_len);

#ifdef __cplusplus
}
#endif
