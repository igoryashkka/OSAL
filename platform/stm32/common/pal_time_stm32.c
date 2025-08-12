#include "pal/pal_time.h"
uint32_t pal_time_now_ms(void){ /* return HAL_GetTick(); */ return 0; }
void     pal_time_delay_ms(uint32_t ms){ (void)ms; /* HAL_Delay(ms); */ }
