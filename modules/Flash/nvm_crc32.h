#pragma once
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
uint32_t nvm_crc32(const void *data, uint32_t len);
#ifdef __cplusplus
}
#endif
