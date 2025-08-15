#pragma once
#include <stddef.h>
#include <stdint.h>
int pal_flash_read(uint32_t addr, void* dst, size_t len);
int pal_flash_write(uint32_t addr, const void* src, size_t len);
