#pragma once
#include "nvm_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int (*init)(void);
    int (*read_latest)(NvmConfigBlob *out);
    int (*commit)(const NvmConfigBlob *in);
    int (*erase_all)(void);
} NvmBackend;

const NvmBackend *nvm_backend_get(void);

#ifdef __cplusplus
}
#endif
