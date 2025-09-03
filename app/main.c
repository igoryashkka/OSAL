#include "module_system.h"
#include "module_blink.h"


int main(void)
{
    init_system();
    blink_init();
    while (1) {
       blink_once();    
    }
}
