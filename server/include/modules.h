#ifndef __MODULES_H__
#define __MODULES_H__

#include "debug.h"

typedef struct {
	int arg_1;
	int arg_2;
} arg_t;

typedef void *(*pthread_func)(void *arg);
pthread_func module_run(void *arg, pthread_func func);

void *pthread_transfer(void *arg);		//ѭ������ M0 ����,���Ϳ�������
void *pthread_uart_cmd(void *arg);		//M0 ���������
void *pthread_sms(void *arg);			//����ģ�����
void *pthread_image(void *arg);			//ͼƬ��ȡ�ͷ���
void *pthread_camera(void *arg);		//Ϊ�����Ч�ʣ�Ϊcamera�����̣߳�\
										���ϵĲɼ�ͼƬ
void *pthread_web(void *arg);			//����web������
void *pthread_waitclit(void *arg);		//�ȴ��ͻ��˽���

char *image_buf_get(int *size);

#endif