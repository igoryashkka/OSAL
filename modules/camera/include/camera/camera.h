#pragma once
#include <stdint.h>
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct camera camera_t;

typedef enum { CAM_OK=0, CAM_ERR=-1 } cam_err_t;
typedef enum { CAM_FMT_RGB565, CAM_FMT_YUV422, CAM_FMT_JPEG } cam_fmt_t;

typedef struct {
  uint16_t width;
  uint16_t height;
  cam_fmt_t fmt;
} cam_config_t;

typedef struct {
  cam_err_t (*init)(camera_t*);
  cam_err_t (*start)(camera_t*);
  cam_err_t (*stop)(camera_t*);
  cam_err_t (*set_config)(camera_t*, const cam_config_t*);
  cam_err_t (*read_frame)(camera_t*, uint8_t* buf, size_t cap, size_t* out_len);
} cam_vtbl_t;

struct camera {
  const cam_vtbl_t* vtbl;
  void* drv; // driver-specific
};

// Generic API
cam_err_t camera_init(camera_t* c);
cam_err_t camera_start(camera_t* c);
cam_err_t camera_stop(camera_t* c);
cam_err_t camera_set_config(camera_t* c, const cam_config_t* cfg);
cam_err_t camera_read_frame(camera_t* c, uint8_t* buf, size_t cap, size_t* out_len);

#ifdef __cplusplus
}
#endif
