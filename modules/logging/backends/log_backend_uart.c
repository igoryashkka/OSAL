#include "logging/log.h"
#include "pal/pal_uart.h"
#include <string.h>

static PalUart* s_uart = 0;

static void uart_write_cb(const char* data, size_t len, void* user){
  (void)user;
  if (s_uart) pal_uart_write(s_uart, (const unsigned char*)data, len);
}

log_backend_t log_backend_make_uart(int uart_index, unsigned baud){
  PalUartConfig cfg = { .baud = baud };
  s_uart = pal_uart_init(uart_index, &cfg);
  log_backend_t b = { .write = uart_write_cb, .user = 0 };
  return b;
}
