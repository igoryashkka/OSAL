#pragma once
#include <stddef.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct eth eth_t;
typedef enum { ETH_OK=0, ETH_ERR=-1 } eth_err_t;

typedef struct {
  uint8_t mac[6];
} eth_config_t;

typedef struct {
  eth_err_t (*init)(eth_t*);
  eth_err_t (*send)(eth_t*, const uint8_t*, size_t);
  int       (*recv)(eth_t*, uint8_t*, size_t, unsigned timeout_ms);
} eth_vtbl_t;

struct eth { const eth_vtbl_t* vtbl; void* drv; };

eth_err_t eth_init(eth_t* e);
eth_err_t eth_send(eth_t* e, const uint8_t* p, size_t n);
int       eth_recv(eth_t* e, uint8_t* p, size_t n, unsigned tmo_ms);

#ifdef __cplusplus
}
#endif
