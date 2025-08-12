#pragma once
#include "logging/log.h"

#ifdef __cplusplus
extern "C" {
#endif

// Factory helpers per backend
log_backend_t log_backend_make_uart(int uart_index, unsigned baud);
log_backend_t log_backend_make_rtt(void);

#ifdef __cplusplus
}
#endif
