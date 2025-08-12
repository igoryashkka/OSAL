#include "logging/log.h"
#include <stdio.h>

static void rtt_write_cb(const char* data, size_t len, void* user){
  (void)user;
  // Placeholder for SEGGER RTT; fallback to stdout on host
  fwrite(data, 1, len, stdout);
}

log_backend_t log_backend_make_rtt(void){
  log_backend_t b = { .write = rtt_write_cb, .user = 0 };
  return b;
}
