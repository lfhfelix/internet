#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>

#include "include/camera.h"
#include "include/modules.h"
#include "include/msg.h"
#include "include/serial.h"
#include "include/m0.h"

#include "include/gsm.h"
#include "include/iconv.h"

#define IMG_MAX_BUF_LEN		65535

//#define DEBUG

extern unsigned int image_size;
extern unsigned char img_buf[50 * 1024];

/*创建线程，启动功能模块*/
pthread_func module_run(void *arg, pthread_func func)
{
	int ret;
	pthread_t tid;
	
	/*创建线程，等待客户端请求*/
	pthread_create(&tid, NULL, func, arg);
	
	ret = pthread_detach(tid);
	if(ret) {
		perror("module_run");
	} else {
		printf("module_run (tid = %d): detach client thread success\n", \
				(int)tid);
	}
	
	return func;
}

/*功能模块：短信模块控制*/
void *pthread_sms(void *arg)
{
	int ret = 0;
	MSG msg;
	
	printf("pthread_sms run...\n");
	
	while(ret >= 0)
	{
		if(ret < 0) {
			printf("CTL_GSM_SMS:error\n");
			break;
		} else {
			ret = sms_run(arg);
			printf("sms_run->restart ...");
		}
	}
	
	printf("pthread_sms : exit\n");
	pthread_exit(NULL);
}

/*启动web服务器*/
void *pthread_web(void *arg)
{
	printf("pthread_web run...\n");
	
	while(1) {
		web_main(arg);
		printf("web->restart ...");
		//sleep(1);
	}
	
	printf("pthread_web : exit\n");
	pthread_exit(NULL);
}

/*功能模块：循环接收 M0 数据*/
void *pthread_transfer(void *arg)
{
	int msg_id = *(int *)arg;
	int ret = 1;
	MSG msg;
	
	printf("pthread_transfer run...\n");
	
	while(ret >= 0) {
		ret = read_m0_run();
		printf("m0->restart ...%d\n", ret);
		sleep(2);
	}
	
	printf("pthread_transfer : exit\n");
	pthread_exit(NULL);
}

/*功能模块：M0控制命令发送*/
void *pthread_uart_cmd(void *arg)
{
	int msg_id = *(int *)arg;
	int ret;
	MSG msg;
	char cmd = 0xff;
	char *pos;
	
	printf("pthread_uart_cmd run...\n");
	
	while(1)
	{
		int fd;
		fd = open_serial(M0_SERIAL);
		if(fd < 0)
		{
			printf("open uart_cmd serial failed\n");
			break;
		}
	
		printf("pthread_uart_cmd serial_fd = %d\n", fd);
		init_serial(fd, 115200, 8, 'N', 1);
		while(1) {
			ret = msg_queue_recv(msg_id, UART_CMD_M0, &msg);
			if(ret < 0) {
				printf("UART_CMD_M0 : error\n");
			} else {
				if(pos = strstr(msg.msg_data, "LED_ON")) printf("LED_ON\n"), cmd = 0x00;
				else if(pos = strstr(msg.msg_data, "LED_OFF")) printf("LED_OFF\n"), cmd = 0x01;
				else if(pos = strstr(msg.msg_data, "BUZZ_ON")) printf("BUZZ_ON\n"), cmd = 0x02;
				else if(pos = strstr(msg.msg_data, "BUZZ_OFF")) printf("BUZZ_OFF\n"), cmd = 0x03;
				else if(pos = strstr(msg.msg_data, "FAN_ON")) printf("FAN_ON\n"), cmd = 0x04;
				else if(pos = strstr(msg.msg_data, "FAN_LOW")) printf("FAN_LOW\n"), cmd = 0x05;
				else if(pos = strstr(msg.msg_data, "FAN_MID")) printf("FAN_MID\n"), cmd = 0x06;
				else if(pos = strstr(msg.msg_data, "FAN_HIGH")) printf("FAN_HIGH\n"), cmd = 0x07;
				else if(pos = strstr(msg.msg_data, "FAN_OFF")) printf("FAN_OFF\n"), cmd = 0x08;
				if(NULL != pos) {
					ret = uart_cmd_send(M0_SEND_SELCET, cmd, fd);
					//printf("uart_cmd_send ret : %d\n", ret);
				}
			}
		}
	}
	
	printf("pthread_uart_cmd : exit\n");
	pthread_exit(NULL);
}

/*启动摄像头,采集图片*/
void *pthread_camera(void *arg)
{
	printf("pthread_camera run...\n");
	
	int ret = 1;
	while(ret >= 0) {
		ret = img_capture(arg);
		printf("camera->restart ...%d\n", ret);
		//sleep(2);
	}
	
	printf("pthread_camera : exit\n");
	pthread_exit(NULL);
}

/*功能模块：图片缓存和发送*/
void *pthread_image(void *arg)
{
	int msg_id = *(int *)arg;
	int connect_fd;
	int ret = 0;
	int count;
	int filesize;
	MSG msg;
	char *imgbuf;
	char sendbuf[IMG_MAX_BUF_LEN];
	char sendlen[IMG_MAX_BUF_LEN];
	
	memset(sendbuf, 0, sizeof(sendbuf));
	
	printf("pthread_image run...\n");
	
	while(1)
	{
		//ret = msg_queue_recv(msg_id, IMG_OK, &msg);
		ret = msg_queue_recv(msg_id, REQ_IMAGE, &msg);
		if(ret < 0) {
			printf("IMG_OK:error\n");
		} else {
			connect_fd = atoi(msg.msg_data);
			imgbuf = image_buf_get(&filesize);
			
#ifdef DEBUG	
			printf("image size : %d\n", filesize);
			
#endif
			sprintf(sendlen, "%d|", filesize);
			sprintf(sendbuf, "%d|", filesize);
			/*拷贝图片到sendbuf*/
			filesize = filesize + strlen(sendlen);
			memcpy(sendbuf + strlen(sendlen), imgbuf, filesize);
			count = 0;
			while(count < filesize) {
				ret = sock_send(connect_fd, sendbuf + count, filesize - count);
				send("image\n");
				if(ret < 0) break;
				count += ret;
			}
		}
	}
	
	printf("pthread_image : exit\n");
	pthread_exit(NULL);
}

char *image_buf_get(int *size)
{
	*size = image_size;
	
	return img_buf;
}
