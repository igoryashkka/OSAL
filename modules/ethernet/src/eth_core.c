#include "net/eth.h"
eth_err_t eth_init(eth_t* e){ return (e&&e->vtbl&&e->vtbl->init)? e->vtbl->init(e):ETH_ERR; }
eth_err_t eth_send(eth_t* e, const uint8_t* p, size_t n){ return (e&&e->vtbl&&e->vtbl->send)? e->vtbl->send(e,p,n):ETH_ERR; }
int       eth_recv(eth_t* e, uint8_t* p, size_t n, unsigned t){ return (e&&e->vtbl&&e->vtbl->recv)? e->vtbl->recv(e,p,n,t):-1; }
