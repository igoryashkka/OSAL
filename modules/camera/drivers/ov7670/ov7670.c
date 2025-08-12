#include "camera/camera.h"
#include "pal/pal_i2c.h"
#include "pal/pal_gpio.h"
#include <string.h>

typedef struct {
  PalI2c* i2c;
  PalGpio reset;
} ov7670_t;

static cam_err_t ov_init(camera_t* c){ (void)c; return CAM_OK; }
static cam_err_t ov_start(camera_t* c){ (void)c; return CAM_OK; }
static cam_err_t ov_stop (camera_t* c){ (void)c; return CAM_OK; }
static cam_err_t ov_set (camera_t* c, const cam_config_t* cfg){ (void)c;(void)cfg; return CAM_OK; }
static cam_err_t ov_read(camera_t* c, uint8_t* b, size_t cap, size_t* out){ (void)c; (void)b; (void)cap; if(out) *out=0; return CAM_OK; }

static const cam_vtbl_t vtbl = { ov_init, ov_start, ov_stop, ov_set, ov_read };

camera_t ov7670_make(PalI2c* i2c, PalGpio reset_pin){
  static ov7670_t state;
  state.i2c = i2c;
  state.reset = reset_pin;
  camera_t cam = { .vtbl = &vtbl, .drv = &state };
  return cam;
}
