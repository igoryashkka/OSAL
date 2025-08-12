#include "pal/pal_i2c.h"
struct PalI2c{};
PalI2c* pal_i2c_init(int index, const PalI2cConfig* cfg){ (void)index;(void)cfg; return 0; }
int     pal_i2c_write(PalI2c* i2c, uint8_t addr, const uint8_t* data, size_t len){ (void)i2c;(void)addr;(void)data; return (int)len; }
int     pal_i2c_read (PalI2c* i2c, uint8_t addr, uint8_t* data, size_t len){ (void)i2c;(void)addr;(void)data; return (int)len; }
