#pragma once
#include <stdint.h>

int pal_flash_unlock(void);
int pal_flash_lock(void);
int pal_flash_erase_page(uint32_t page_addr);
int pal_flash_program_halfword(uint32_t addr, uint16_t data);
