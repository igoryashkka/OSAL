#include "pal/pal_time.h"
#include <time.h>
#include <stdint.h>
#include <unistd.h>

uint32_t pal_time_now_ms(void){
  struct timespec ts; clock_gettime(CLOCK_MONOTONIC, &ts);
  return (uint32_t)(ts.tv_sec*1000u + ts.tv_nsec/1000000u);
}
void pal_time_delay_ms(uint32_t ms){
  usleep(ms*1000u);
}
