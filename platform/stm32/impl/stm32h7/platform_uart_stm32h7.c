#include "Platform_UART/platform_uart.h"

/* TODO: Implement for STM32H7 */
#include <stddef.h>
void Platform_UART_Init(uint32_t baud){(void)baud;}
int Platform_UART_Write(const uint8_t* data,int len){(void)data;return len;}
