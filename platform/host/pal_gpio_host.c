#include "pal/pal_gpio.h"
#include <stdlib.h>
struct G{int port;int pin;int level;}; typedef struct G G;
PalGpio pal_gpio_acquire(int port,int pin){ G* g=(G*)malloc(sizeof(G)); g->port=port; g->pin=pin; g->level=0; return g; }
int pal_gpio_config(PalGpio g, PalGpioDir dir, int pull){ (void)g;(void)dir;(void)pull; return 0; }
int pal_gpio_write(PalGpio g, int level){ ((G*)g)->level=level; return 0; }
int pal_gpio_read (PalGpio g){ return ((G*)g)->level; }
int pal_gpio_toggle(PalGpio g){ ((G*)g)->level = !((G*)g)->level; return ((G*)g)->level; }
