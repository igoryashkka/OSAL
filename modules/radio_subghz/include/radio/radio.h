#pragma once
#include <stddef.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct radio radio_t;
typedef enum { RADIO_OK=0, RADIO_ERR=-1 } radio_err_t;

typedef struct {
  uint32_t freq_hz;
  uint32_t bitrate;
  int8_t   tx_power_dbm;
} radio_config_t;

typedef struct {
  radio_err_t (*init)(radio_t*);
  radio_err_t (*set_config)(radio_t*, const radio_config_t*);
  radio_err_t (*tx)(radio_t*, const uint8_t*, size_t);
  int         (*rx)(radio_t*, uint8_t*, size_t, unsigned timeout_ms);
} radio_vtbl_t;

struct radio {
  const radio_vtbl_t* vtbl;
  void* drv;
};

radio_err_t radio_init(radio_t* r);
radio_err_t radio_set_config(radio_t* r, const radio_config_t* cfg);
radio_err_t radio_tx(radio_t* r, const uint8_t* data, size_t len);
int         radio_rx(radio_t* r, uint8_t* data, size_t cap, unsigned timeout_ms);

#ifdef __cplusplus
}
#endif
