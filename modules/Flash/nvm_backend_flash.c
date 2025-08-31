#include "nvm_config.h"
#include "nvm_backend.h"
#include "nvm_crc32.h"
#include <string.h>

typedef struct __attribute__((packed)) {
    uint32_t magic;
    uint32_t version;
    uint32_t seq;
    uint32_t crc32;  /* CRC over blob only */
    NvmConfigBlob blob;
} NvmFlashSlot;

#define REGION_START   (NVM_FLASH_REGION_BASE)
#define REGION_BYTES   (NVM_FLASH_NUM_PAGES * NVM_FLASH_PAGE_SIZE)
#define REGION_END     (REGION_START + REGION_BYTES)

static int fl_init(void) { return NVM_STATUS_OK; }

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

static int erase_region(void) {
    if (HAL_FLASH_Unlock() != HAL_OK) return NVM_STATUS_LOCKED;
    FLASH_EraseInitTypeDef ei = {
        .TypeErase = FLASH_TYPEERASE_PAGES,
        .PageAddress = REGION_START,
        .NbPages = NVM_FLASH_NUM_PAGES
    };
    uint32_t err = 0;
    if (HAL_FLASHEx_Erase(&ei, &err) != HAL_OK) { HAL_FLASH_Lock(); return NVM_STATUS_IO_ERROR; }
    HAL_FLASH_Lock();
    return NVM_STATUS_OK;
}

static int program_buffer(uint32_t dst, const void *src_buf, size_t len) {
    /* Program in units required by platform (2 or 4 bytes) */
    const uint8_t *src = (const uint8_t*)src_buf;
    if (HAL_FLASH_Unlock() != HAL_OK) return NVM_STATUS_LOCKED;

    /* Write aligned units */
    size_t i = 0;
    while (i + NVM_FLASH_PROG_UNIT_BYTES <= len) {
        uint32_t word = 0xFFFFFFFFu;
        memcpy(&word, src + i, NVM_FLASH_PROG_UNIT_BYTES);
        if (HAL_FLASH_Program(NVM_HAL_FLASH_PROG_TYPE, dst + i, word) != HAL_OK) {
            HAL_FLASH_Lock();
            return NVM_STATUS_IO_ERROR;
        }
        i += NVM_FLASH_PROG_UNIT_BYTES;
    }

    /* Tail (pad with 0xFF) */
    if (i < len) {
        uint32_t word = 0xFFFFFFFFu;
        memcpy(&word, src + i, len - i);
        if (HAL_FLASH_Program(NVM_HAL_FLASH_PROG_TYPE, dst + i, word) != HAL_OK) {
            HAL_FLASH_Lock();
            return NVM_STATUS_IO_ERROR;
        }
    }

    HAL_FLASH_Lock();
    return NVM_STATUS_OK;
}

static int fl_commit(const NvmConfigBlob *in) {
    NvmFlashSlot w;
    memset(&w, 0xFF, sizeof(w));
    w.magic   = NVM_MAGIC_VALUE;
    w.version = NVM_STRUCT_VERSION;
    w.seq     = HAL_GetTick();
    w.blob    = *in;

    /* Refresh inner blob header and CRC */
    w.blob.magic   = NVM_MAGIC_VALUE;
    w.blob.version = NVM_STRUCT_VERSION;
    w.blob.crc32   = 0u;
    const uint8_t *payload = (const uint8_t*)&w.blob.brightness;
    w.blob.crc32 = nvm_crc32(payload, sizeof(NvmConfigBlob) - (payload - (const uint8_t*)&w.blob));

    /* Slot CRC over blob */
    w.crc32 = nvm_crc32(&w.blob, sizeof(w.blob));

    /* Find first erased slot */
    uint32_t target = 0;
    for (uint32_t a = REGION_START; a + sizeof(NvmFlashSlot) <= REGION_END; a += sizeof(NvmFlashSlot)) {
        if (*(const uint32_t*)a == 0xFFFFFFFFu) { target = a; break; }
    }
    if (target == 0) {
        int er = erase_region();
        if (er != NVM_STATUS_OK) return er;
        target = REGION_START;
    }

    return program_buffer(target, &w, sizeof(w));
}

static int fl_erase_all(void) { return erase_region(); }

#if NVM_USE_DATA_EEPROM
/* If EEPROM is used, we still keep flash backend compiled (optional), but backend_get should resolve to EEPROM file. */
const NvmBackend *nvm_backend_get(void);
#else
static const NvmBackend s_backend = {
    .init = fl_init,
    .read_latest = fl_read_latest,
    .commit = fl_commit,
    .erase_all = fl_erase_all,
};
const NvmBackend *nvm_backend_get(void) { return &s_backend; }
#endif
