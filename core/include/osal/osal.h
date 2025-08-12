#pragma once
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef void* osal_thread_t;
typedef void* osal_mutex_t;
typedef void* (*osal_thread_fn)(void*);

int  osal_init(void);
int  osal_start_thread(osal_thread_fn fn, void* arg, const char* name, unsigned stack_bytes, int prio, osal_thread_t* out);
void osal_delay_ms(uint32_t ms);
osal_mutex_t osal_mutex_create(void);
void osal_mutex_lock(osal_mutex_t m);
void osal_mutex_unlock(osal_mutex_t m);

#ifdef __cplusplus
}
#endif
