#include "net/eth.h"
#include "pal/pal_spi.h"
typedef struct { PalSpi* spi; } w5500_t;
static eth_err_t init(eth_t* e){ (void)e; return ETH_OK; }
static eth_err_t send(eth_t* e, const uint8_t* p, size_t n){ (void)e;(void)p;(void)n; return ETH_OK; }
static int       recv(eth_t* e, uint8_t* p, size_t n, unsigned t){ (void)e;(void)p;(void)n;(void)t; return 0; }
static const eth_vtbl_t vtbl={init,send,recv};
eth_t w5500_make(PalSpi* spi){ static w5500_t s; s.spi=spi; eth_t e={.vtbl=&vtbl,.drv=&s}; return e; }
