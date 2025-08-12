#include "camera/camera.h"
cam_err_t camera_init(camera_t* c){ return (c&&c->vtbl&&c->vtbl->init)? c->vtbl->init(c):CAM_ERR; }
cam_err_t camera_start(camera_t* c){ return (c&&c->vtbl&&c->vtbl->start)? c->vtbl->start(c):CAM_ERR; }
cam_err_t camera_stop (camera_t* c){ return (c&&c->vtbl&&c->vtbl->stop )? c->vtbl->stop (c):CAM_ERR; }
cam_err_t camera_set_config(camera_t* c, const cam_config_t* cfg){ return (c&&c->vtbl&&c->vtbl->set_config)? c->vtbl->set_config(c,cfg):CAM_ERR; }
cam_err_t camera_read_frame(camera_t* c, uint8_t* b, size_t cap, size_t* out){ return (c&&c->vtbl&&c->vtbl->read_frame)? c->vtbl->read_frame(c,b,cap,out):CAM_ERR; }
