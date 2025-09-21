#include "stm32f1xx_hal.h"
#include <string.h>  

int pal_flash_init(void)
{
    return NVM_STATUS_OK;
}


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


static int scan_latest(NvmFlashSlot *out, uint32_t *addr_out) {
    uint32_t best_seq = 0;
    int found = 0;
    for (uint32_t a = REGION_START; a + sizeof(NvmFlashSlot) <= REGION_END; a += sizeof(NvmFlashSlot)) {
        const NvmFlashSlot *p = (const NvmFlashSlot*)a;
        if (p->magic != NVM_MAGIC_VALUE || p->version != NVM_STRUCT_VERSION) continue;
        uint32_t saved = p->crc32;
        uint32_t calc  = nvm_crc32(&p->blob, sizeof(p->blob));
        if (calc != saved) continue;
        if (!found || p->seq > best_seq) { *out = *p; *addr_out = a; best_seq = p->seq; found = 1; }
    }
    return found ? NVM_STATUS_OK : NVM_STATUS_NOT_FOUND;
}

static int fl_read_latest(NvmConfigBlob *out) {
    NvmFlashSlot s; uint32_t addr = 0;
    int st = scan_latest(&s, &addr);
    if (st != NVM_STATUS_OK) return st;
    *out = s.blob;
    return NVM_STATUS_OK;
}