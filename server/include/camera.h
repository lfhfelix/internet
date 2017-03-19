#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <linux/videodev2.h>

#include "debug.h"

int img_capture(void *arg);
void conv_rgb_to_jpeg_exit(void);
int conv_rgb_to_jpeg_work(void *rgb, void *jpeg, unsigned int width, unsigned int height, unsigned int bpp, int quality);
void conv_rgb_to_jpeg_init(void);
void rgb_to_bmp(void *rgb, void *bmp, unsigned int width, unsigned int height);
void yuv_to_rgb(void *yuv, void *rgb, unsigned int width, unsigned int height);
void camera_exit(int fd);
int camera_stop(int fd);
int camera_eqbuf(int fd, unsigned int index);
int camera_dqbuf(int fd, void **buf, unsigned int *size, unsigned int *index);
int camera_start(int fd);
int camera_init(char *devpath, unsigned int *width, unsigned int *height, unsigned *size);

#endif