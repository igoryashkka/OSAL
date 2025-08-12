#pragma once
#include <stddef.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct PalI2c PalI2c;
typedef struct {
  uint32_t hz;
} PalI2cConfig;

PalI2c* pal_i2c_init(int index, const PalI2cConfig* cfg);
int     pal_i2c_write(PalI2c* i2c, uint8_t addr, const uint8_t* data, size_t len);
int     pal_i2c_read (PalI2c* i2c, uint8_t addr, uint8_t* data, size_t len);

#ifdef __cplusplus
}
#endif
