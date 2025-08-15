#include "stm32f1xx_hal.h"
#include <string.h>  

/* Minimal stubs – adjust to your flash layout as needed */
int pal_flash_read(uint32_t addr, void* dst, size_t len)
{
    memcpy(dst, (const void*)addr, len);
    return 0;
}

int pal_flash_write(uint32_t addr, const void* src, size_t len)
{
    HAL_FLASH_Unlock();
    const uint8_t* s = (const uint8_t*)src;
    for (size_t i = 0; i < len; i += 2) {
        uint16_t half = s[i];
        if (i + 1 < len) half |= ((uint16_t)s[i+1]) << 8;
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, addr + i, half) != HAL_OK) {
            HAL_FLASH_Lock();
            return -1;
        }
    }
    HAL_FLASH_Lock();
    return 0;
}
