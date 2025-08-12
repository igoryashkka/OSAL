#pragma once
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  LOG_LEVEL_ERROR=0,
  LOG_LEVEL_WARN =1,
  LOG_LEVEL_INFO =2,
  LOG_LEVEL_DEBUG=3,
} log_level_t;

typedef struct {
  void (*write)(const char* data, size_t len, void* user);
  void* user;
} log_backend_t;

void log_init(const log_backend_t* backend);
void log_set_level(log_level_t lvl);
void log_printf(log_level_t lvl, const char* fmt, ...);
void log_hexdump(log_level_t lvl, const void* data, size_t len);

#ifdef __cplusplus
}
#endif
