#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "nvm_config.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    NVM_STATUS_OK = 0,
    NVM_STATUS_NOT_FOUND = 1,
    NVM_STATUS_CRC_MISMATCH = 2,
    NVM_STATUS_IO_ERROR = -1,
    NVM_STATUS_NO_SPACE = -2,
    NVM_STATUS_LOCKED = -3,
    NVM_STATUS_INVALID = -4
} NvmStatus;

typedef struct __attribute__((packed)) {
    uint32_t magic;
    uint32_t version;
    uint32_t crc32;
    uint8_t  brightness;
    uint8_t  volume;
    uint16_t timeout_s;
    uint32_t device_id;
} NvmConfigBlob;

typedef struct {
    NvmConfigBlob working;
    NvmConfigBlob committed;
    bool          has_valid_commit;
    uint32_t      next_due_ms;
} NvmContext;

static inline void nvm_fill_defaults(NvmConfigBlob *cfg) {
    cfg->magic      = NVM_MAGIC_VALUE;
    cfg->version    = NVM_STRUCT_VERSION;
    cfg->crc32      = 0u;
    cfg->brightness = 50u;
    cfg->volume     = 10u;
    cfg->timeout_s  = 60u;
    cfg->device_id  = 0x12345678u;
}

#ifdef __cplusplus
}
#endif
