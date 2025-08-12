#include "radio/radio.h"
#include "pal/pal_spi.h"
#include "pal/pal_gpio.h"
typedef struct { PalSpi* spi; PalGpio nss; } rfm69_t;
static radio_err_t init (radio_t* r){ (void)r; return RADIO_OK; }
static radio_err_t setc (radio_t* r, const radio_config_t* c){ (void)r;(void)c; return RADIO_OK; }
static radio_err_t tx   (radio_t* r, const uint8_t* d, size_t l){ (void)r;(void)d;(void)l; return RADIO_OK; }
static int         rx   (radio_t* r, uint8_t* d, size_t c, unsigned t){ (void)r;(void)d;(void)c;(void)t; return 0; }
static const radio_vtbl_t vtbl = { init,setc,tx,rx };
radio_t rfm69_make(PalSpi* spi, PalGpio nss){ static rfm69_t s; s.spi=spi; s.nss=nss; radio_t r={.vtbl=&vtbl,.drv=&s}; return r; }
