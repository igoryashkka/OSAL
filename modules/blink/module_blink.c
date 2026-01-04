#include "Platform_GPIO/Platform_gpio.h"
#include "module_blink.h"



void blink_init(){
  GPIO_InitLed();
}

void blink_once(void){
    GPIO_ToggleLed();
    for(int i = 0 ; i < 5000000;i++);
    GPIO_ToggleLed();
    for(int i = 0 ; i < 5000000;i++);
}
