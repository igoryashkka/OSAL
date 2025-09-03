#include "pal_gpio.h"
#include "module_blink.h"



void blink_init(){
  pal_gpio_init_led();
}

void blink_once(void) {
    pal_gpio_toggle_led();
    pal_delay(5000000);
    pal_gpio_toggle_led();
    pal_delay(5000000);
}
