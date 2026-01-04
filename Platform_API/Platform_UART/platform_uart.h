#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void Platform_UART_Init(uint32_t baud);
int  Platform_UART_Write(const uint8_t* data, int len);

#ifdef __cplusplus
}
#endif
