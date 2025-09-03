#pragma once
#include "nvm_types.h"
#ifdef __cplusplus
extern "C" {
#endif

void nvm_init(NvmContext *ctx);
const NvmConfigBlob* nvm_get(const NvmContext *ctx);

/**
 * @brief Force an immediate commit of ctx->working to NVM, even if unchanged.
 * @return NVM_STATUS_OK on success; negative on backend error.
 */
int  nvm_commit(NvmContext *ctx);

/**
 * @brief Check if ctx->working differs from last committed; commit only if changed.
 *        (Alias for the write-if-changed behavior from your flowchart.)
 * @return NVM_STATUS_OK if no change or commit succeeded; negative on error.
 */
int  nvm_check_commit(NvmContext *ctx);

/* Legacy name kept for compatibility (same as nvm_check_commit). */
int  nvm_commit_if_changed(NvmContext *ctx);

void nvm_tick(NvmContext *ctx, uint32_t now_ms);

#ifdef __cplusplus
}
#endif
