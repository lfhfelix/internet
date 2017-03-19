#ifndef __MODULES_H__
#define __MODULES_H__

#include "debug.h"

typedef struct {
	int arg_1;
	int arg_2;
} arg_t;

typedef void *(*pthread_func)(void *arg);
pthread_func module_run(void *arg, pthread_func func);

void *pthread_transfer(void *arg);		//循环接收 M0 数据,或发送控制命令
void *pthread_uart_cmd(void *arg);		//M0 控制命令发送
void *pthread_sms(void *arg);			//短信模块控制
void *pthread_image(void *arg);			//图片获取和发送
void *pthread_camera(void *arg);		//为了提高效率，为camera启动线程，\
										不断的采集图片
void *pthread_web(void *arg);			//启动web服务器
void *pthread_waitclit(void *arg);		//等待客户端接入

char *image_buf_get(int *size);

#endif