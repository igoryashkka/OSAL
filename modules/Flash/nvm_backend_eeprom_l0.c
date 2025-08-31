#include "nvm_config.h"
#if NVM_HAS_DATA_EEPROM
#include "nvm_backend.h"
#include "nvm_crc32.h"
#include <stddef.h>

#define NVM_EEPROM_SLOT_ADDR   (NVM_DATA_EEPROM_BASE)

static int eep_init(void) { return NVM_STATUS_OK; }

static int eep_read_latest(NvmConfigBlob *out) {
    const NvmConfigBlob *rom = (const NvmConfigBlob*)NVM_EEPROM_SLOT_ADDR;
    if (rom->magic != NVM_MAGIC_VALUE || rom->version != NVM_STRUCT_VERSION) return NVM_STATUS_NOT_FOUND;
    *out = *rom;
    NvmConfigBlob tmp = *out;
    uint32_t saved = tmp.crc32;
    tmp.crc32 = 0u;
    const uint8_t *payload = (const uint8_t*)&tmp.brightness;
    uint32_t calc = nvm_crc32(payload, sizeof(NvmConfigBlob) - (payload - (const uint8_t*)&tmp));
    return (calc == saved) ? NVM_STATUS_OK : NVM_STATUS_CRC_MISMATCH;
}

static int eep_commit(const NvmConfigBlob *in) {
    NvmConfigBlob w = *in;
    w.magic   = NVM_MAGIC_VALUE;
    w.version = NVM_STRUCT_VERSION;
    w.crc32   = 0u;
    const uint8_t *payload = (const uint8_t*)&w.brightness;
    w.crc32 = nvm_crc32(payload, sizeof(NvmConfigBlob) - (payload - (const uint8_t*)&w));

    if (HAL_FLASHEx_DATAEEPROM_Unlock() != HAL_OK) return NVM_STATUS_LOCKED;
    const uint32_t *src = (const uint32_t*)&w;
    for (size_t i = 0, addr = NVM_EEPROM_SLOT_ADDR; i < sizeof(NvmConfigBlob)/4; ++i, addr += 4) {
        if (HAL_FLASHEx_DATAEEPROM_Program(NVM_HAL_EEPROM_PROG_TYPE, addr, src[i]) != HAL_OK) {
            HAL_FLASHEx_DATAEEPROM_Lock();
            return NVM_STATUS_IO_ERROR;
        }
    }
    HAL_FLASHEx_DATAEEPROM_Lock();
    return NVM_STATUS_OK;
}

static int eep_erase_all(void) {
    if (HAL_FLASHEx_DATAEEPROM_Unlock() != HAL_OK) return NVM_STATUS_LOCKED;
    for (uint32_t a = NVM_EEPROM_SLOT_ADDR; a < NVM_EEPROM_SLOT_ADDR + sizeof(NvmConfigBlob); a += 4) {
        if (HAL_FLASHEx_DATAEEPROM_Program(NVM_HAL_EEPROM_PROG_TYPE, a, 0xFFFFFFFFu) != HAL_OK) {
            HAL_FLASHEx_DATAEEPROM_Lock();
            return NVM_STATUS_IO_ERROR;
        }
    }
    HAL_FLASHEx_DATAEEPROM_Lock();
    return NVM_STATUS_OK;
}

static const NvmBackend s_backend = {
    .init = eep_init,
    .read_latest = eep_read_latest,
    .commit = eep_commit,
    .erase_all = eep_erase_all,
};
const NvmBackend *nvm_backend_get(void) { return &s_backend; }
#endif
