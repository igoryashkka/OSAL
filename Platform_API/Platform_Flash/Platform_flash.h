#pragma once
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int FSH_Read(uint32_t addr, void* dst, size_t len);
int FSH_Write(uint32_t addr, const void* src, size_t len);

#ifdef __cplusplus
}
#endif
