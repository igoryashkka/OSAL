#include "osal/osal.h"
#include <stdint.h>
int  osal_init(void){ return 0; }
int  osal_start_thread(osal_thread_fn fn, void* arg, const char* name, unsigned stack, int prio, osal_thread_t* out){
  (void)fn;(void)arg;(void)name;(void)stack;(void)prio;(void)out; return -1; // not supported in bare-metal
}
void osal_delay_ms(uint32_t ms){ (void)ms; /* HAL_Delay(ms); */ }
osal_mutex_t osal_mutex_create(void){ return (osal_mutex_t)0; }
void osal_mutex_lock(osal_mutex_t m){ (void)m; }
void osal_mutex_unlock(osal_mutex_t m){ (void)m; }
