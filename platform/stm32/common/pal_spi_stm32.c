#include "pal/pal_spi.h"
struct PalSpi{};
PalSpi* pal_spi_init(int index, const PalSpiConfig* cfg){ (void)index;(void)cfg; return 0; }
int     pal_spi_transfer(PalSpi* s, const uint8_t* tx, uint8_t* rx, size_t len){ (void)s;(void)tx;(void)rx; return (int)len; }
