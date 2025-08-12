#pragma once
#include <stddef.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct PalSpi PalSpi;
typedef struct {
  uint32_t hz;
  uint8_t  mode; // SPI mode 0..3
} PalSpiConfig;

PalSpi* pal_spi_init(int index, const PalSpiConfig* cfg);
int     pal_spi_transfer(PalSpi* s, const uint8_t* tx, uint8_t* rx, size_t len);

#ifdef __cplusplus
}
#endif
