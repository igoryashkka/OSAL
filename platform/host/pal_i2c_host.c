#include "pal/pal_i2c.h"
#include <stdlib.h>
struct PalI2c{int i; unsigned hz;};
PalI2c* pal_i2c_init(int index, const PalI2cConfig* cfg){ PalI2c* i2c=(PalI2c*)malloc(sizeof(PalI2c)); i2c->i=index; i2c->hz=cfg?cfg->hz:100000; return i2c; }
int pal_i2c_write(PalI2c* i2c, uint8_t addr, const uint8_t* data, size_t len){ (void)i2c;(void)addr;(void)data; return (int)len; }
int pal_i2c_read (PalI2c* i2c, uint8_t addr, uint8_t* data, size_t len){ (void)i2c;(void)addr;(void)data; return (int)len; }
