#include "pal/pal_gpio.h"
int     pal_gpio_config(PalGpio g, PalGpioDir dir, int pull){ (void)g;(void)dir;(void)pull; return 0; }
PalGpio pal_gpio_acquire(int port, int pin){ (void)port;(void)pin; return 0; }
int     pal_gpio_write(PalGpio g, int level){ (void)g;(void)level; return 0; }
int     pal_gpio_read (PalGpio g){ (void)g; return 0; }
int     pal_gpio_toggle(PalGpio g){ (void)g; return 0; }
