#include "pal/pal_uart.h"
#include <stdio.h>
#include <stdlib.h>

struct PalUart { int index; unsigned baud; };

PalUart* pal_uart_init(int index, const PalUartConfig* cfg){
  (void)cfg;
  PalUart* u = (PalUart*)malloc(sizeof(PalUart));
  u->index = index; u->baud = cfg?cfg->baud:115200;
  return u;
}
int pal_uart_write(PalUart* u, const unsigned char* data, size_t len){
  (void)u;
  return (int)fwrite(data, 1, len, stdout);
}
