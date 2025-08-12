#include "osal/osal.h"
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

int osal_init(void){ return 0; }
int osal_start_thread(osal_thread_fn fn, void* arg, const char* name, unsigned stack, int prio, osal_thread_t* out){
  (void)name;(void)stack;(void)prio;
  pthread_t* t = (pthread_t*)malloc(sizeof(pthread_t));
  if(pthread_create(t, NULL, fn, arg)!=0){ free(t); return -1; }
  if(out) *out = t;
  return 0;
}
void osal_delay_ms(uint32_t ms){ usleep(ms*1000u); }
osal_mutex_t osal_mutex_create(void){ pthread_mutex_t* m=(pthread_mutex_t*)malloc(sizeof(pthread_mutex_t)); pthread_mutex_init(m,NULL); return m; }
void osal_mutex_lock(osal_mutex_t m){ pthread_mutex_lock((pthread_mutex_t*)m); }
void osal_mutex_unlock(osal_mutex_t m){ pthread_mutex_unlock((pthread_mutex_t*)m); }
