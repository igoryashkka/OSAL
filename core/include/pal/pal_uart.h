#pragma once
#include <stdint.h>

typedef struct {
  void *instance;       // USART1/2… casted inside impl
  uint32_t baud;
  uint8_t  tx_dma;      // 0/1 if you later want DMA
  uint8_t  rx_dma;
} pal_uart_cfg_t;

int pal_uart_init(const pal_uart_cfg_t *cfg);
int pal_uart_write(const uint8_t *buf, uint32_t len);
int pal_uart_read(uint8_t *buf, uint32_t len, uint32_t timeout_ms);
