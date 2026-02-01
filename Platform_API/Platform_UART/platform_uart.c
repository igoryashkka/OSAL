#include "platform_uart.h"

#ifndef BUILD_FOR_STM8
__attribute__((weak)) void Platform_UART_Init(uint32_t baud) { (void)baud; }
__attribute__((weak)) int  Platform_UART_Write(const uint8_t* data, int len)
{
    (void)data;
    return len; /* pretend written */
}
__attribute__((weak)) int  Platform_UART_Read(uint8_t* buffer, int max_len)
{
    (void)buffer;
    (void)max_len;
    return 0; /* no data available */
}
#endif
