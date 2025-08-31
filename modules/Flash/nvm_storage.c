#include "nvm_storage.h"
#include "nvm_backend.h"
#include <string.h>

static const NvmBackend *s_backend = NULL; /* s_ = static (file-scope) */

static int blobs_equal(const NvmConfigBlob *a, const NvmConfigBlob *b) {
    NvmConfigBlob ta = *a, tb = *b;
    ta.crc32 = 0u; tb.crc32 = 0u;
    return memcmp(&ta, &tb, sizeof(NvmConfigBlob)) == 0;
}


const NvmConfigBlob* nvm_get(const NvmContext *ctx) { return &ctx->working; }



/*  alias for write-if-changed ----- */
int nvm_check_commit(NvmContext *ctx) {
    return nvm_commit_if_changed(ctx);
}

void nvm_tick(NvmContext *ctx, uint32_t now_ms) {
    if (now_ms < ctx->next_due_ms) return;
    (void)nvm_commit_if_changed(ctx);
    ctx->next_due_ms = now_ms + NVM_PERIODIC_MS;
}
