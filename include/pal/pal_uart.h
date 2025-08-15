#pragma once
#include <stdint.h>
void pal_uart_init(uint32_t baud);
int  pal_uart_write(const uint8_t* data, int len);
