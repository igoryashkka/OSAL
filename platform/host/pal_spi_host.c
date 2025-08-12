#include "pal/pal_spi.h"
#include <stdlib.h>
struct PalSpi{int i; unsigned hz;};
PalSpi* pal_spi_init(int index, const PalSpiConfig* cfg){ PalSpi* s=(PalSpi*)malloc(sizeof(PalSpi)); s->i=index; s->hz=cfg?cfg->hz:1000000; return s; }
int pal_spi_transfer(PalSpi* s, const uint8_t* tx, uint8_t* rx, size_t len){ (void)s;(void)tx;(void)rx; return (int)len; }
