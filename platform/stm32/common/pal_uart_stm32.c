#include "pal/pal_uart.h"
// #include "stm32xx_hal.h" // include real HAL for your family

struct PalUart{ int idx; unsigned baud; /* UART_HandleTypeDef huart; */ };
PalUart* pal_uart_init(int index, const PalUartConfig* cfg){
  (void)index;(void)cfg;
  // TODO: init UARTx and store handle
  return (PalUart*)0; // return real handle in your implementation
}
int pal_uart_write(PalUart* u, const uint8_t* data, size_t len){
  (void)u;(void)data;(void)len;
  // HAL_UART_Transmit(&u->huart, (uint8_t*)data, len, 100);
  return (int)len;
}
