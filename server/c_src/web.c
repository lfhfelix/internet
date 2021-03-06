#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "include/serial.h"
#include "include/sqlite.h"
#include "include/msg.h"
#include "include/m0.h"
#include "include/web.h"

//#define	DEBUG	1

#define	MAX_BACKLOG	32
#define	MAX_BUF_LEN	(1024*20)
#define	RES_BUF_LEN	(1204*20)

extern unsigned char img_buf[50 * 1024];	
extern unsigned int image_size;
extern env_t env_msg;

/*m0控制*/
int msg_id;
MSG msg;

/*	
LED_OFF
BUZZ_ON
BUZZ_OFF
FAN_ON
FAN_OFF
*/

void *web_main(void *arg)
{
	int ret;
	int listenfd;
	int opt = 1;
	int connfd;
	struct sockaddr_in servaddr;
	struct sockaddr_in cliaddr;
	socklen_t addrlen;
	/*m0控制,121行举例*/
	int msg_id_temp = *(int *)arg;
	msg_id = msg_id_temp;
	
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd == -1) {
		perror("webserver->socket");
	}
	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	memset(&servaddr, 0, sizeof(struct sockaddr_in));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(8080);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	ret = bind(listenfd, (struct sockaddr *)&servaddr, sizeof(struct sockaddr_in));
	if (ret == -1) {
		perror("webserver->bind");
		close(listenfd);
	}

	ret = listen(listenfd, MAX_BACKLOG);
	if (ret == -1) {
		perror("webserver->listen");
		close(listenfd);
	}

	while (1) {
		addrlen = sizeof(struct sockaddr_in);
		memset(&cliaddr, 0, sizeof(struct sockaddr_in));
		connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &addrlen);
		if (connfd == -1) {
			perror("webserver->accept");
			break;
		}
#ifdef DEBUG
		printf("webserver->accept: a new client is comming, and the connfd is %d\n", connfd);
#endif

		http_process(connfd);
		close(connfd);
	}

	close(listenfd);
}
/*******************************************************************/
/*******************************************************************/
void http_process(int connfd)
{
	int ret;
	char request[MAX_BUF_LEN];
	char response[MAX_BUF_LEN];

	memset(request, 0, sizeof(request));
	ret = read(connfd, request, MAX_BUF_LEN);
	if (ret <= 0) {
		if (ret == 0)
			printf("webserver->read: end-of-file\n");
		else
			perror("webserver->read");
		return;
	}
#ifdef DEBUG
	printf("webserver->read: the request is\n%s", request);
#endif

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	if (strstr(request, "GET /index.html") != NULL) {
		int ret;
		FILE *fp;
		struct stat filestat;
		int filesize;
		char status[] = "HTTP/1.0 200 OK\r\n";		//状态行
		char response_header[] = "Server: fsc100\r\nContent-Type: text/html\r\n\r\n"; //响应头标
		char response_text[RES_BUF_LEN];			//响应数据

#ifdef DEBUG
		printf("webserver->http: get index.html\n");
#endif
		fp = fopen("index.html", "rb");
		if (fp == NULL) {
			perror("webserver->fopen");
			return;
		}

		ret = stat("index.html", &filestat);
		if (ret == -1) {
			perror("webserver->stat");
			return;
		}

		filesize = filestat.st_size;
		memset(response_text, 0, sizeof(response_text));
		if (fread(response_text, filesize, 1, fp) != 1) {
			fprintf(stderr, "webserver->fread: fread failure\n");
			fclose(fp);
			return;
		}

		memset(response, 0, sizeof(response));
		strcat(response, status);
		strcat(response, response_header);
		strcat(response, response_text);
#ifdef DEBUG
		printf("webserver->http: the response is write index.html\n%s\n", response);
#endif
		ret = write(connfd, response, strlen(response));
		if (ret == -1) {
			perror("webserver->write ");
			fclose(fp);
			return;
		}

		fclose(fp);
	}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
if (strstr(request, "GET /main.html") != NULL) {
		int ret;
		FILE *fp;
		struct stat filestat;
		int filesize;
		char status[] = "HTTP/1.0 200 OK\r\n";		//状态行
		char response_header[] = "Server: fsc100\r\nContent-Type: text/html\r\n\r\n"; //响应头标
		char response_text[RES_BUF_LEN];			//响应数据

#ifdef DEBUG
		printf("webserver->http: get main.html\n");
#endif
		fp = fopen("main/main.html", "rb");
		if (fp == NULL) {
			perror("webserver->fopen main.html");
			return;
		}

		ret = stat("main/main.html", &filestat);
		if (ret == -1) {
			perror("webserver->stat");
			return;
		}

		filesize = filestat.st_size;
		memset(response_text, 0, sizeof(response_text));
		if (fread(response_text, filesize, 1, fp) != 1) {
			fprintf(stderr, "webserver->fread: fread failure\n");
			fclose(fp);
			return;
		}

		memset(response, 0, sizeof(response));
		strcat(response, status);
		strcat(response, response_header);
		strcat(response, response_text);
#ifdef DEBUG
		printf("webserver->http: the response is write main.html\n%s\n", response);
#endif
		ret = write(connfd, response, strlen(response));
		if (ret == -1) {
			perror("webserver->write");
			fclose(fp);
			return;
		}

		fclose(fp);
	}
		
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

if (strstr(request, "GET /env.html") != NULL) {
		int ret;
		FILE *fp;
		struct stat filestat;
		int filesize;
		char status[] = "HTTP/1.0 200 OK\r\n";		//状态行
		char response_header[] = "Server: fsc100\r\nContent-Type: text/html\r\n\r\n"; //响应头标
		char response_text[RES_BUF_LEN];					//响应数据

#ifdef DEBUG
		printf("webserver->http: get html\n");
#endif
		fp = fopen("main/env.html", "rb");
		if (fp == NULL) {
			perror("webserver->fopen env.html");
			return;
		}
		
		ret = stat("main/env.html", &filestat);
		if (ret == -1) {
			perror("webserver->stat");
			return;
		}

		filesize = filestat.st_size;
		
#ifdef DEBUG
		printf("[web webserver]env.html filesize is:%d\n",filesize);
#endif

		memset(response_text, 0, sizeof(response_text));
		if (fread(response_text, filesize, 1, fp) != 1) {
			fprintf(stderr, "webserver->fread: fread failure\n");
			fclose(fp);
			return;
		}
		memset(response, 0, sizeof(response));
		strcat(response, status);
		strcat(response, response_header);
		strcat(response, response_text);
#ifdef DEBUG
		printf("webserver->http: the response is write env.html\n%s\n", response);
#endif
		ret = write(connfd, response, strlen(response));
		
		if (ret == -1) {
			perror("webserver->write");
			fclose(fp);
			return;
		}
	//	printf("write env.html success\n");
		fclose(fp);
	}
		
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

if (strstr(request, "GET /introduce.html") != NULL) {
		int ret;
		FILE *fp;
		struct stat filestat;
		int filesize;
		char status[] = "HTTP/1.0 200 OK\r\n";		//状态行
		char response_header[] = "Server: fsc100\r\nContent-Type: text/html\r\n\r\n"; //响应头标
		char response_text[RES_BUF_LEN];			//响应数据

#ifdef DEBUG
		printf("webserver->http: get html\n");
#endif
		fp = fopen("main/introduce.html", "rb");
		if (fp == NULL) {
			perror("webserver->fopen introduce.html");
			return;
		}

		ret = stat("main/introduce.html", &filestat);
		if (ret == -1) {
			perror("webserver->stat");
			return;
		}

		filesize = filestat.st_size;
		memset(response_text, 0, sizeof(response_text));
		if (fread(response_text, filesize, 1, fp) != 1) {
			fprintf(stderr, "webserver->fread: fread failure\n");
			fclose(fp);
			return;
		}

		memset(response, 0, sizeof(response));
		strcat(response, status);
		strcat(response, response_header);
		strcat(response, response_text);
#ifdef DEBUG
		printf("webserver->http: the response is write introduce.html\n%s\n", response);
#endif
		ret = write(connfd, response, strlen(response));
		if (ret == -1) {
			perror("webserver->write");
			fclose(fp);
			return;
		}

		fclose(fp);
	}
	
	
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if (strstr(request, "GET /video.html") != NULL) {
		int ret;
		FILE *fp;
		struct stat filestat;
		int filesize;
		char status[] = "HTTP/1.0 200 OK\r\n";		//状态行
		char response_header[] = "Server: fsc100\r\nContent-Type: text/html\r\n\r\n"; //响应头标
		char response_text[RES_BUF_LEN];					//响应数据

#ifdef DEBUG
		printf("webserver->http: get html\n");
#endif
		fp = fopen("main/video.html", "rb");
		if (fp == NULL) {
			perror("webserver->fopen video.html");
			return;
		}

		ret = stat("main/video.html", &filestat);
		if (ret == -1) {
			perror("webserver->stat");
			return;
		}

		filesize = filestat.st_size;

#ifdef DEBUG
		printf("[web webserver]video.html filesize is:%d\n",filesize);
#endif

		memset(response_text, 0, sizeof(response_text));
		if (fread(response_text, filesize, 1, fp) != 1) {
			fprintf(stderr, "webserver->fread: fread failure\n");
			fclose(fp);
			return;
		}

		memset(response, 0, sizeof(response));
		strcat(response, status);
		strcat(response, response_header);
		strcat(response, response_text);
#ifdef DEBUG
		printf("webserver->http: the response is write video.html\n%s\n", response);
#endif
		ret = write(connfd, response, strlen(response));
		if (ret == -1) {
			perror("webserver->write");
			fclose(fp);
			return;
		}

		fclose(fp);
	}
/***************背景1*************/
#if 1
if (strstr(request, "GET /?beijing1") != NULL) {
		static int loop = 0;
		FILE *fp;
		int filesize;
		int count;
		struct stat filestat;
		char *imgbuf;
		char status[] = "HTTP/1.0 200 OK\r\n";
		char response_header[] = "Server: fsc100\r\nContent-Type: image/jpeg\r\nContent-Length: ";
		char length[32];

#ifdef DEBUG
		printf("server->http: get image\n");
#endif
		
		fp = fopen("image/1.jpg", "rb");
		
		if (fp == NULL) {
			perror("server->fopen");
			return;
		}
		ret = stat("image/1.jpg", &filestat);
		if (ret == -1) {
			perror("server->stat");
			return;
		}
		
		filesize = filestat.st_size;
	 
		imgbuf = malloc(filesize);
		if (imgbuf == NULL) {
			fprintf(stderr, "server->malloc: malloc failure\n");
			fclose(fp);
			return;
		}

		if (fread(imgbuf, filesize, 1, fp) != 1) {
			fprintf(stderr, "server->fread: fread failure\n");
			fclose(fp);
			return;
		}

		memset(response, 0, sizeof(response));
		strcat(response, status);
		strcat(response, response_header);
		snprintf(length, sizeof(length), "%d", filesize);
		strcat(response, length);
		strcat(response, "\r\n\r\n");
#ifdef DEBUG
		printf("server->http: the response is\n%s\n", response);
#endif
		ret = write(connfd, response, strlen(response));
		if (ret == -1) {
			perror("server->write");
			free(imgbuf);
			fclose(fp);
			return;
		}

		ret = write(connfd, imgbuf, filesize);
		if (ret == -1) {
			perror("server->write");
			free(imgbuf);
			fclose(fp);
			return;
		}

#ifdef DEBUG
		printf("server->write: write %d bytes success\n", filesize);
#endif
		free(imgbuf);
		fclose(fp);
	}

#endif 
	
/***************背景2*************/
#if 1
if (strstr(request, "GET /?beijing2") != NULL) {
		static int loop = 0;
		FILE *fp;
		int filesize;
		int count;
		struct stat filestat;
		char *imgbuf;
		char status[] = "HTTP/1.0 200 OK\r\n";
		char response_header[] = "Server: fsc100\r\nContent-Type: image/jpeg\r\nContent-Length: ";
		char length[32];

#ifdef DEBUG
		printf("server->http: get image\n");
#endif
		
		fp = fopen("image/2.jpg", "rb");
		
		if (fp == NULL) {
			perror("server->fopen");
			return;
		}
		ret = stat("image/2.jpg", &filestat);
		if (ret == -1) {
			perror("server->stat");
			return;
		}
		
		filesize = filestat.st_size;
	 
		imgbuf = malloc(filesize);
		if (imgbuf == NULL) {
			fprintf(stderr, "server->malloc: malloc failure\n");
			fclose(fp);
			return;
		}

		if (fread(imgbuf, filesize, 1, fp) != 1) {
			fprintf(stderr, "server->fread: fread failure\n");
			fclose(fp);
			return;
		}

		memset(response, 0, sizeof(response));
		strcat(response, status);
		strcat(response, response_header);
		snprintf(length, sizeof(length), "%d", filesize);
		strcat(response, length);
		strcat(response, "\r\n\r\n");
#ifdef DEBUG
		printf("server->http: the response is\n%s\n", response);
#endif
		ret = write(connfd, response, strlen(response));
		if (ret == -1) {
			perror("server->write");
			free(imgbuf);
			fclose(fp);
			return;
		}

		ret = write(connfd, imgbuf, filesize);
		if (ret == -1) {
			perror("server->write");
			free(imgbuf);
			fclose(fp);
			return;
		}

#ifdef DEBUG
		printf("server->write: write %d bytes success\n", filesize);
#endif
		free(imgbuf);
		fclose(fp);
	}

#endif 
	
/****************************************************************/	
/************************具体命令解析****************************/

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~登录~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/	
#if 1
	if (strstr(request, "POST /index.html?login") != NULL) {
		int ret;
		char status[] = "HTTP/1.0 200 OK\r\n";		//状态行
		char response_header[] = "Server: fsc100\r\nContent-Type: text/html\r\n\r\n";   //响应头标
		char response_text[32] = "";			//响应数据
		sqlite3 *mydb;	
		DB_T data;
		
		char *name,*password,*end;			
		name = strstr(request,"1|");
		password = strstr(name,"2|");
		end  = strstr(password,"3|");
		
		char data_buf[128] = {0};
		memset(&data, 0, sizeof(data));
		memcpy(data_buf,name+2,password-name-2);
		strcpy(data.name, data_buf);

		char data_buf2[128] = {0};
		memcpy(data_buf2,password+2,end-password-2);
		strcpy(data.text, data_buf2);
		printf("%s\n%s\n", data.name, data.text);
	
		mydb = mydb_open();
		sql_search(mydb, USR_DB, &data);

		char *p;
		p = strstr(data.text, "ok");
//		printf("_____%s\n",data.text);
		if(p != NULL)
		{
			strcat(response_text, "true");
			printf("login sucess!\n");
		}
		else
		{
			strcat(response_text, "false");
			printf("login fail!\n");
		}
		
		memset(response, 0, sizeof(response));
		strcat(response, status);
		strcat(response, response_header);
		strcat(response, response_text);
#ifdef DEBUG
		printf("webserver->http: the response is\n%s\n", response);
#endif
		ret = write(connfd, response, strlen(response));
		if (ret == -1) {
			perror("webserver->write");
			return;
		}
		
	}	
#endif

/*****************注册*****************/	
#if 1
	if (strstr(request, "POST /index.html?regist") != NULL) {
		int ret;
		char status[] = "HTTP/1.0 200 OK\r\n";		//状态行
		char response_header[] = "Server: fsc100\r\nContent-Type: text/html\r\n\r\n";   //响应头标
		char response_text[32] = "";			//响应数据
		
		sqlite3 *mydb;					//定义数据库句柄
		DB_T data;						//定义 DB_T数据结构变量data
		memset(&data, 0, sizeof(data));
		
		char *name,*password,*end;			
		name = strstr(request,"1|");
		password = strstr(name,"2|");
		end  = strstr(password,"3|");
		
		char data_buf[128] = {0};		//定义装name的数组
		
		memcpy(data_buf,name+2,password-name-2);
		strcpy(data.name, data_buf);

		char data_buf2[128] = {0};		//定义装password的数组
		memcpy(data_buf2,password+2,end-password-2);
		strcpy(data.text, data_buf2);
		printf("%s\n%s\n", data.name, data.text);
	
		mydb = mydb_open();
		int tag;
		tag = sql_insert(mydb, USR_DB, &data);

		if(tag == 0)
		{
			strcat(response_text, "true");
			printf("regist sucess!\n");
		}
		else
		{
			strcat(response_text, "false");
			printf("regist fail!\n");
		}
		
		memset(response, 0, sizeof(response));
		strcat(response, status);
		strcat(response, response_header);
		strcat(response, response_text);
#ifdef DEBUG
		printf("webserver->http: the response is\n%s\n", response);
#endif
		ret = write(connfd, response, strlen(response));
		if (ret == -1) {
			perror("webserver->write");
			return;
		}
		
	}	
#endif



/*******************获取图片***********************/
	if (strstr(request, "GET /?action=snapshot") != NULL) {
		int filesize;									//web服务器中用来存储图片的大小
		//char *imgbuf;									//指向web服务器中用来存储图片的首地址
		char status[] = "HTTP/1.0 200 OK\r\n";			//状态行
		char response_header[] = "Server: fsc100\r\nContent-Type: image/jpeg\r\nContent-Length: "; //响应头标
		char length[32];								//响应数据

#ifdef DEBUG
		printf("webserver->http: get image\n");
#endif
		char imgbuf[50 * 1024];
		
		//pthread_mutex_lock(&mutex);	//互斥锁上锁	
		filesize = image_size;			   //保存图片大小
	
		//imgbuf = malloc(filesize);
		if (imgbuf == NULL) {
			fprintf(stderr, "webserver->malloc: malloc failure\n");
			return;
		}

		memcpy(imgbuf,img_buf,filesize);
	//	strncpy(imgbuf,img_buf,filesize);					//将接收的图片读入imgbuf指向的空间
		
		//pthread_mutex_unlock(&mutex);	//互斥锁解锁
														  //filesize读取图片的大小
		memset(response, 0, sizeof(response));
		strcat(response, status);
		strcat(response, response_header);
		snprintf(length, sizeof(length), "%d", filesize);
		strcat(response, length);
		strcat(response, "\r\n\r\n");
#ifdef DEBUG
		printf("webserver->http: the response is\n%s\n", response);
#endif
		
		ret = write(connfd, response, strlen(response));
		if (ret == -1) {
			perror("webserver->write");
			//free(imgbuf);
			return;
		}

		ret = write(connfd, imgbuf, filesize);
	//	printf("[web webserver]image/1.jpg filesize is:%d\n",filesize);
		if (ret == -1) {
			perror("webserver->write");
			//free(imgbuf);
			return;
		}
		
#ifdef DEBUG
		printf("webserver->write: write %d bytes success\n", filesize);
#endif
		//free(imgbuf);
		
	}
	
/*******************获取背景音乐***********************/
	if (strstr(request, "GET bgsound/Richard Clayderma - 绿袖子.mp3") != NULL) {
		FILE *fp;
		int filesize;
		struct stat filestat;
		char *imgbuf;									//用来存储背景音乐的首地址
		char status[] = "HTTP/1.0 200 OK\r\n";			//状态行
		char response_header[] = "Server: fsc100\r\nContent-Type: image/jpeg\r\nContent-Length: "; //响应头标
		char length[32];								//响应数据

		fp = fopen("main/bgsound/Richard Clayderma - 绿袖子.mp3", "rb");
		if (fp == NULL) {
			perror("server->fopen");
			return;
		}
		ret = stat("main/bgsound/Richard Clayderma - 绿袖子.mp3", &filestat);
		if (ret == -1) {
			perror("server->stat");
			return;
		}
		filesize = filestat.st_size;

		imgbuf = malloc(filesize);
		if (imgbuf == NULL) {
			fprintf(stderr, "server->malloc: malloc failure\n");
			fclose(fp);
			return;
		}

		if (fread(imgbuf, filesize, 1, fp) != 1) {
			fprintf(stderr, "server->fread: fread failure\n");
			fclose(fp);
			return;
		}

		memset(response, 0, sizeof(response));
		strcat(response, status);
		strcat(response, response_header);
		snprintf(length, sizeof(length), "%d", filesize);
		strcat(response, length);
		strcat(response, "\r\n\r\n");
		
#ifdef DEBUG
		printf("webserver->http: the response is\n%s\n", response);
#endif
		
		ret = write(connfd, response, strlen(response));
		if (ret == -1) {
			perror("webserver->write");
			free(imgbuf);
			fclose(fp);
			return;
		}

		ret = write(connfd, imgbuf, filesize);
	//	printf("[web webserver]image/1.jpg filesize is:%d\n",filesize);
		if (ret == -1) {
			perror("webserver->write");
			free(imgbuf);
			fclose(fp);
			return;
		}
		
#ifdef DEBUG
		printf("webserver->write: write %d bytes success\n", filesize);
#endif
		free(imgbuf);
		fclose(fp);
		
	}
	
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/**********************LED控制命令的解析*********************/		
	if (strstr(request, "POST /index.html?led=") != NULL) {
		int ret;
		char status[] = "HTTP/1.0 200 OK\r\n";
		char response_header[] = "Server: fsc100\r\nContent-Type: text/html\r\n\r\n";
		char response_text[32] = "";
		
		msg.msg_type = UART_CMD_M0;
		
		if (strstr(request, "led=on") != NULL) {
			
			strcpy(msg.msg_data ,"LED_ON");
			msg_queue_insert(msg_id, msg);			
			printf("webserver->http: led on\n");
			
		//	strcat(response_text, msg.msg_data);
		}

		if (strstr(request, "led=off") != NULL) {
			strcpy(msg.msg_data ,"LED_OFF");
			msg_queue_insert(msg_id, msg);				
			printf("webserver->http: led off\n");
			
		//	strcat(response_text, msg.msg_data);
		}

		memset(response, 0, sizeof(response));
		strcat(response, status);
		strcat(response, response_header);
		strcat(response, response_text);
#ifdef DEBUG
		printf("webserver->http: the response is\n%s\n", response);
#endif
		ret = write(connfd, response, strlen(response));
		if (ret == -1) {
			perror("webserver->write");
			return;
		}
	}

/**********************LED显示命令的解析*********************/		
	if (strstr(request, "POST /index.html?ledshow") != NULL) {
		int ret;
		char status[] = "HTTP/1.0 200 OK\r\n";
		char response_header[] = "Server: fsc100\r\nContent-Type: text/html\r\n\r\n";
		char response_text[32] = "";
				
		if(env_msg.led)
			sprintf(response_text, "%s", "led on");
		else
			sprintf(response_text, "%s", "led off");
	
		memset(response, 0, sizeof(response));
		strcat(response, status);
		strcat(response, response_header);
		strcat(response, response_text);
#ifdef DEBUG
		printf("webserver->http: the response is\n%s\n", response);
#endif
		ret = write(connfd, response, strlen(response));
		if (ret == -1) {
			perror("webserver->write");
			return;
		}
	}
	
	
/********************风扇控制命令的解析********************/	
	if (strstr(request, "POST /index.html?fan=") != NULL) {
		int ret;
		char status[] = "HTTP/1.0 200 OK\r\n";
		char response_header[] = "Server: fsc100\r\nContent-Type: text/html\r\n\r\n";
		char response_text[32] = "";				//风扇状态的提示信息
		
		msg.msg_type = UART_CMD_M0;
		
		if (strstr(request, "fan=on") != NULL) {
			//msg.msg_data = FAN_ON;
			strcpy(msg.msg_data ,"FAN_ON");
			msg_queue_insert(msg_id, msg);	
			printf("webserver->http:fan off\n");

		//	strcat(response_text, "off");
		}
		if (strstr(request, "fan=off") != NULL) {
			//msg.msg_data = FAN_OFF;
			strcpy(msg.msg_data ,"FAN_OFF");
			msg_queue_insert(msg_id, msg);	
			printf("webserver->http:fan on\n");
			
		//	strcat(response_text, "on");
		}

		memset(response, 0, sizeof(response));
		strcat(response, status);
		strcat(response, response_header);
		strcat(response, response_text);
#ifdef DEBUG
		printf("webserver->http: the response is\n%s\n", response);
#endif
		ret = write(connfd, response, strlen(response));
		if (ret == -1) {
			perror("webserver->write");
			return;
		}
	}

/********************风扇显示命令的解析********************/	
	if (strstr(request, "POST /index.html?fanshow") != NULL) {
		int ret;
		char status[] = "HTTP/1.0 200 OK\r\n";
		char response_header[] = "Server: fsc100\r\nContent-Type: text/html\r\n\r\n";
		char response_text[32] = "";				//风扇状态的提示信息
			
		if(env_msg.fan)
			sprintf(response_text, "%s", "fan on");
		else
			sprintf(response_text, "%s", "fan off");
	
		memset(response, 0, sizeof(response));
		strcat(response, status);
		strcat(response, response_header);
		strcat(response, response_text);
#ifdef DEBUG
		printf("webserver->http: the response is\n%s\n", response);
#endif
		ret = write(connfd, response, strlen(response));
		if (ret == -1) {
			perror("webserver->write");
			return;
		}
	}
	
	
/******************蜂鸣器控制命令的解析*********************/
	if (strstr(request, "POST /index.html?buzzer=") != NULL) {
		int ret;
		char status[] = "HTTP/1.0 200 OK\r\n";
		char response_header[] = "Server: fsc100\r\nContent-Type: text/html\r\n\r\n";
		char response_text[32] = "";			//蜂鸣器状态的提示信息
		
		msg.msg_type = UART_CMD_M0;
		
		if (strstr(request, "buzzer=on") != NULL) {
			//msg.msg_data = BUZZ_ON;
			strcpy(msg.msg_data ,"BUZZ_ON");
			msg_queue_insert(msg_id, msg);	
			printf("webserver->http:buzzer off\n");
		
		//	strcat(response_text, "off");
		}
		if (strstr(request, "buzzer=off") != NULL) {
		//	msg.msg_data = BUZZ_OFF;
			strcpy(msg.msg_data ,"BUZZ_OFF");
			msg_queue_insert(msg_id, msg);	
			printf("webserver->http:buzzer on\n");

		//	strcat(response_text, "on");
		}
		
		memset(response, 0, sizeof(response));
		strcat(response, status);
		strcat(response, response_header);
		strcat(response, response_text);
#ifdef DEBUG
		printf("webserver->http: the response is\n%s\n", response);
#endif
		ret = write(connfd, response, strlen(response));
		if (ret == -1) {
			perror("webserver->write");
			return;
		}
	}

/******************蜂鸣器状态显示命令的解析*********************/
	if (strstr(request, "POST /index.html?buzzershow") != NULL) {
		int ret;
		char status[] = "HTTP/1.0 200 OK\r\n";
		char response_header[] = "Server: fsc100\r\nContent-Type: text/html\r\n\r\n";
		char response_text[32] = "";			//蜂鸣器状态的提示信息
				
		if(env_msg.buzz)
			sprintf(response_text, "%s", "buzz on");
		else
			sprintf(response_text, "%s", "buzz off");
	
		memset(response, 0, sizeof(response));
		strcat(response, status);
		strcat(response, response_header);
		strcat(response, response_text);
#ifdef DEBUG
		printf("webserver->http: the response is\n%s\n", response);
#endif
		ret = write(connfd, response, strlen(response));
		if (ret == -1) {
			perror("webserver->write");
			return;
		}
	}
	
	
/***************温度显示的解析*******************/	
	if (strstr(request, "POST /index.html?tempshow") != NULL) {
		int ret;
		char status[] = "HTTP/1.0 200 OK\r\n";
		char response_header[] = "Server: fsc100\r\nContent-Type: text/html\r\n\r\n";
		char response_text[32];		//应该从服务器获得环境参数

#ifdef DEBUG
		//printf("webserver->http: tempshow\n");
#endif
		
/*		if (strstr(request, "|OFF") != NULL) {
			printf("server->http: tempshow on\n");
			sprintf(response_text, "%d %s", env_msg.tmp,"°C");
		}

		if (strstr(request, "|ON") != NULL) {
			printf("server->http: tempshow off\n");
			strcat(response_text, "tempshow off");
		}
*/
		sprintf(response_text, "%d %s", env_msg.tmp,"°C");	
		//printf("server->http: tempshow on\n");		
		memset(response, 0, sizeof(response));
		strcat(response, status);
		strcat(response, response_header);
		strcat(response, response_text);
#ifdef DEBUG
		printf("webserver->http: the response is tempshow\n%s\n", response);
#endif
		ret = write(connfd, response, strlen(response));
		if (ret == -1) {
			perror("webserver->write");
			return;
		}
	}
	
/**************湿度显示的解析****************/	
	if (strstr(request, "POST /index.html?rhshow") != NULL) {
		int ret;
		char status[] = "HTTP/1.0 200 OK\r\n";
		char response_header[] = "Server: fsc100\r\nContent-Type: text/html\r\n\r\n";
		char response_text[32];		//应该从服务器获得环境参数

#ifdef DEBUG
		printf("webserver->http: rhshow\n");
#endif
		sprintf(response_text, "%d %s", env_msg.hum,"%RH");
		memset(response, 0, sizeof(response));
		strcat(response, status);
		strcat(response, response_header);
		strcat(response, response_text);
#ifdef DEBUG
		printf("webserver->http: the response is rhshow\n%s\n", response);
#endif
		ret = write(connfd, response, strlen(response));
		if (ret == -1) {
			perror("webserver->write");
			return;
		}
	}

/**************光照显示的解析****************/	
	if (strstr(request, "POST /index.html?lightshow") != NULL) {
		int ret;
		char status[] = "HTTP/1.0 200 OK\r\n";
		char response_header[] = "Server: fsc100\r\nContent-Type: text/html\r\n\r\n";
		char response_text[32];					//应该从服务器获得环境参数
#ifdef	DEBUG
	printf("webserver->http: lightshow\n");
#endif	
		sprintf(response_text, "%d %s", env_msg.light,"cd");
		memset(response, 0, sizeof(response));
		strcat(response, status);
		strcat(response, response_header);
		strcat(response, response_text);
#ifdef DEBUG
		printf("webserver->http: the response is lightshow\n%s\n", response);
#endif
		ret = write(connfd, response, strlen(response));
		if (ret == -1) {
			perror("webserver->write");
			return;
		}
	}
	
	
/***************环境参数设置命令的解析*************/	
	if (strstr(request, "POST /index.html?env_set") != NULL) {
		int ret;
		char status[] = "HTTP/1.0 200 OK\r\n";
		char response_header[] = "Server: fsc100\r\nContent-Type: text/html\r\n\r\n";
		char response_text[32] = "";			//响应数据
		printf("webserver->http: env_set:\n");
		
		if(strstr(request,"tmp_max") != NULL){
			strncat(response_text, request,34);
			printf("tmp_max: %s\n",response_text+32);
		}
		if(strstr(request,"tmp_min") != NULL){
			strncat(response_text, request,34);
			printf("tmp_mix: %s\n",response_text+32);
		}
		if(strstr(request,"rh_max") != NULL){
			strncat(response_text, request,33);
			printf("rh_max: %s\n",response_text+31);
		}
		if(strstr(request,"rh_min") != NULL){
			strncat(response_text, request,33);
			printf("rh_mix: %s\n",response_text+31);
		}
		
		memset(response, 0, sizeof(response));
		strcat(response, status);
		strcat(response, response_header);
		strcat(response, response_text);
#ifdef DEBUG
		printf("webserver->http: the response is env_set\n%s\n", response);
#endif
		ret = write(connfd, response, strlen(response));
		if (ret == -1) {
			perror("webserver->write");
			return;
		}
	}
	/****/
}
