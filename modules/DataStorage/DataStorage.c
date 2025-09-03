#include <stdint.h>
#include <string.h>

#define MAX_RADIO_DATA_SIZE 256

void nvm_init(NvmContext *ctx) {
    memset(ctx, 0, sizeof(*ctx));
    s_backend = nvm_backend_get();
    if (s_backend && s_backend->init) (void)s_backend->init();

    NvmConfigBlob tmp;
    int st = s_backend && s_backend->read_latest ? s_backend->read_latest(&tmp) : NVM_STATUS_NOT_FOUND;
    if (st == NVM_STATUS_OK) {
        ctx->committed = tmp;
        ctx->working   = tmp;
        ctx->has_valid_commit = true;
    } else {
        nvm_fill_defaults(&ctx->working);
        ctx->has_valid_commit = false;
    }
    ctx->next_due_ms = 0u;
}

int nvm_commit(NvmContext *ctx) {
    int rc = s_backend->commit(&ctx->working);
    if (rc == NVM_STATUS_OK) {
        ctx->committed = ctx->working;
        ctx->has_valid_commit = true;
    }
    return rc;
}

int nvm_commit_if_changed(NvmContext *ctx) {
    if (ctx->has_valid_commit && blobs_equal(&ctx->working, &ctx->committed))
        return NVM_STATUS_OK;
    return nvm_commit(ctx);
}