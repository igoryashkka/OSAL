#include "logging/log.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

static log_backend_t g_backend = {0};
static log_level_t   g_level = LOG_LEVEL_INFO;

void log_init(const log_backend_t* backend) {
  if (backend) g_backend = *backend;
}

void log_set_level(log_level_t lvl) { g_level = lvl; }

static void emit(const char* s, size_t n){
  if (g_backend.write) g_backend.write(s, n, g_backend.user);
}

void log_printf(log_level_t lvl, const char* fmt, ...) {
  if (lvl > g_level) return;
  char buf[256];
  int off = 0;
  const char* tag = (lvl==LOG_LEVEL_ERROR)?"E":(lvl==LOG_LEVEL_WARN)?"W":(lvl==LOG_LEVEL_INFO)?"I":"D";
  off += snprintf(buf+off, sizeof(buf)-off, "[%s] ", tag);
  va_list ap; va_start(ap, fmt);
  off += vsnprintf(buf+off, sizeof(buf)-off, fmt, ap);
  va_end(ap);
  if (off >= (int)sizeof(buf)) off = (int)sizeof(buf)-1;
  buf[off++] = '\n';
  emit(buf, (size_t)off);
}

void log_hexdump(log_level_t lvl, const void* data, size_t len) {
  if (lvl > g_level) return;
  const unsigned char* p = (const unsigned char*)data;
  char line[80];
  for (size_t i=0;i<len;i+=16){
    int off = snprintf(line, sizeof(line), "[D] %04X: ", (unsigned)i);
    for (size_t j=0;j<16;j++){
      if (i+j<len) off += snprintf(line+off, sizeof(line)-off, "%02X ", p[i+j]);
      else off += snprintf(line+off, sizeof(line)-off, "   ");
    }
    off += snprintf(line+off, sizeof(line)-off, " |");
    for (size_t j=0;j<16 && i+j<len;j++){
      unsigned char c = p[i+j];
      line[off++] = (c>=32 && c<127)? c : '.';
    }
    line[off++] = '|';
    line[off++] = '\n';
    emit(line, (size_t)off);
  }
}
