#pragma once
#include <stdint.h>

typedef enum { PAL_CLK_SRC_HSI, PAL_CLK_SRC_HSE_8MHz, PAL_CLK_SRC_HSE_16MHz } pal_clk_src_t;

typedef struct {
  pal_clk_src_t src;
  uint32_t sysclk_hz;   // e.g. 72'000'000
  uint32_t ahb_hz;
  uint32_t apb1_hz;
  uint32_t apb2_hz;
} pal_clock_cfg_t;

int pal_clock_init(const pal_clock_cfg_t *cfg);  // 0 = OK
uint32_t pal_clock_get_sysclk(void);
