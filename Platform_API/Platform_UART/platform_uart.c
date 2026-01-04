#include "platform_uart.h"

__attribute__((weak)) void UART_Init(uint32_t baud) { (void)baud; }
__attribute__((weak)) int  UART_Write(const uint8_t* data, int len)
{
    (void)data;
    return len; /* pretend written */
}
