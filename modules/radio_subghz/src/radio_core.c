#include "radio/radio.h"
radio_err_t radio_init(radio_t* r){ return (r&&r->vtbl&&r->vtbl->init)? r->vtbl->init(r):RADIO_ERR; }
radio_err_t radio_set_config(radio_t* r, const radio_config_t* cfg){ return (r&&r->vtbl&&r->vtbl->set_config)? r->vtbl->set_config(r,cfg):RADIO_ERR; }
radio_err_t radio_tx(radio_t* r, const uint8_t* d, size_t l){ return (r&&r->vtbl&&r->vtbl->tx)? r->vtbl->tx(r,d,l):RADIO_ERR; }
int         radio_rx(radio_t* r, uint8_t* d, size_t c, unsigned t){ return (r&&r->vtbl&&r->vtbl->rx)? r->vtbl->rx(r,d,c,t):-1; }
