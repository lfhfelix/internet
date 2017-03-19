#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <sys/stat.h>

#include "include/entry.h"

#define PORT	28888
//#define DEBUG

unsigned int image_size;
unsigned char img_buf[50 * 1024];

/*客户端请求*/
void *pthread_requst(void *arg)
{
	MSG msg;
	arg_t arg_temp = *(arg_t*)arg;
	int connect_fd = arg_temp.arg_1;
	int msg_id = arg_temp.arg_2;
	int	recv_len = 0;
	char msgbuf[RECV_MAX_LEN];
	
	//read(connect_fd, msgbuf, RECV_MAX_LEN);
	int sec = 8;
	int count = 0;
	while((recv_len = sock_recv(connect_fd, msgbuf, &sec)) >= 0) {
		memset(&msg, 0, sizeof(msg));
		if(-1 == msg_pack(&msg, msgbuf, recv_len)) {
			sock_send(connect_fd, "error", strlen("error"));
		}
		
		if(0 == recv_len) {
			msg.msg_type = 10086;
			printf("timeout counter : %d\n", count);
			if(count == 16) {
				count = 0;
				//msg.msg_type = REQ_QUIT;
			}
			count ++;
		}
		switch (msg.msg_type) {
		case REQ_LOGIN:	
			client_login(connect_fd, msg.msg_data);
			break;
		case REQ_QUIT:
			client_quit(connect_fd);
			printf("client->pthread : exit succeed\n");
			pthread_exit(NULL);
			break;
		case REQ_IMAGE:
			msg.msg_type = REQ_IMAGE;
			sprintf(msg.msg_data, "%d", connect_fd);    		 //将connect_fd赋值给msg.msg_data，并插入消息队列中
			msg_queue_insert(msg_id, msg);
			break;
		case GET_ENV_M0:
			msg.msg_type = GET_ENV_M0;
			sprintf(msg.msg_data, "%d", connect_fd);    		 //将connect_fd赋值给msg.msg_data，并插入消息队列中
			char env_buf[20];
			sprintf(env_buf, "4|%d|%d|%d|%d|%d|%d|", env_msg.tmp, env_msg.hum, \
					env_msg.light, env_msg.led, env_msg.fan, env_msg.buzz);
			sock_send(connect_fd, env_buf, 20);
			break;
		case UART_CMD_M0:
			msg.msg_type = UART_CMD_M0;
			msg_queue_insert(msg_id, msg);
			break;
		default:
			break;
		/*......*/
		}
	}
	
	close(connect_fd);
	printf("client->pthread : exit succeed\n");
	pthread_exit(NULL);
}

/*客户端退出*/
void client_quit(int connect_fd)
{	
	sock_send(connect_fd, "exit success", strlen("exit success"));
	
	close(connect_fd);
}

/*客户端登录*/
void client_login(int connect_fd, const char *user_msg)
{
	DB_T db_t;
	char *pos;
	sqlite3 *my_db;
	
	pos = strstr(user_msg,"|");
		if(NULL == pos) {
		printf("usrmsg->error\n");
	}
	
	memset(&db_t, 0, sizeof(db_t));
	strncpy(db_t.name, user_msg, pos - user_msg);
	strcpy(db_t.text, pos + 1);
	my_db = mydb_open();
	
#ifdef DEBUG	
	printf("%s\n", db_t.name);
	printf("%s\n", db_t.text);
#endif

	/*查找到匹配的条目时，db_t.text被设置为ok*/
	if(0 == sql_search(my_db, USR_DB, &db_t)) {
		printf("a client login success\n");
		sock_send(connect_fd, db_t.text, strlen(db_t.text));
	}
	else {
		printf("a client login failed\n");
		sock_send(connect_fd, "error", strlen("error"));
	}

#ifdef DEBUG
	printf("%s\n", db_t.text);
#endif
}

/*功能模块：等待客户端连接*/
void *wait_client(void *arg)
{
	int listen_fd;
	int msg_id;
	int connect_fd;
	int ret;
	pthread_t tid;
	
	arg_t arg_temp = *(arg_t *)arg;
	listen_fd = arg_temp.arg_1;
	msg_id = arg_temp.arg_2;

	while(1) {
		/*等待与客户端建立连接*/
		connect_fd = sock_getconfd(28888, listen_fd);
		if(-1 == connect_fd) {
			printf("client->connect : failed\n");
			break;
		}
		
		arg_temp.arg_1 = connect_fd;
		
		/*创建线程，等待客户端请求*/
		pthread_create(&tid, NULL, pthread_requst, &arg_temp);
		ret = pthread_detach(tid);
		if(ret) {
			perror("server->thread");
			close(connect_fd);
		} else {
			printf("server->thread:detach client thread success\n");
		}
	}
}

void *pthread_wait(void *arg)
{
	printf("pthread_wait run...\n");

	while(1) {
		wait_client(arg);
		printf("pthread_wait->restart ...");
		sleep(1);
	}
	
	printf("pthread_wait : exit\n");
	pthread_exit(NULL);
}

//模拟环境信息数据刷新
void *simul_env(void *arg)
{
	while(1) {
		sleep(2);
		srand(time(0));
		env_msg.tmp = rand()%100;
		env_msg.hum = rand()%100;
		env_msg.light = rand()%1000 + 1;
		env_msg.led = rand()%2;
		env_msg.fan = rand()%2;
		env_msg.buzz = rand()%2;
	}
	
	pthread_exit(NULL);
}

//模拟图片刷新
void *simul_image(void *arg)
{	
	FILE *fp;
	int ret;
	int filesize;
	struct stat filestat;
	char imgbuf[50 * 1024];
	static int loop = 0;
	
	while(1) {
		bzero(imgbuf, 0);
		if(loop == 0) {
			fp = fopen("1.jpg", "rb");
			if (fp == NULL) {
				perror("simul_image->fopen");
			}
			ret = stat("1.jpg", &filestat);
			if (ret == -1) {
				perror("simul_image->stat");
				fclose(fp);
			}
			sleep(1);
		} else { 
			fp = fopen("2.jpg", "rb");
			if (fp == NULL) {
				perror("simul_image->fopen");
			}
			ret = stat("2.jpg", &filestat);
			if (ret == -1) {
				perror("simul_image->stat");
				fclose(fp);
			}
			sleep(1);
		}
		
		filesize = filestat.st_size;
		if (fread(imgbuf, filesize, 1, fp) != 1) {
			fprintf(stderr, "simul_image->fread: fread failure\n");
			fclose(fp);
		}
		image_size = filesize;
		memcpy(img_buf, imgbuf, image_size);
		fclose(fp);
		loop ++;
		loop %= 2;
	}
}

int main(int argc, char *argv[])
{
	int listen_fd;
	int msg_id;
	int args;
	sqlite3 *my_db;
	
	/*待插入数据库的用户信息，用于测试*/
	DB_T db_usr = {
		.name = "hello",
		.text = "123456",
	};
	
	//环境信息默认值设置
	env_msg.tmp = 11;
	env_msg.hum = 22;
	env_msg.light = 88;
	env_msg.led = 1;
	env_msg.fan = 0;
	env_msg.buzz = 0;
	env_msg.tmp_max = 40;
	env_msg.tmp_min = 20;
	env_msg.hum_max = 110;
	env_msg.hum_min = 60;
	
	/*数据库初始化*/
	my_db = mydb_open();
	create_table(my_db, USR_DB);
	sql_insert(my_db, USR_DB, &db_usr);
	sql_display(my_db, USR_DB);
	mydb_close(my_db);
	
	/*创建消息队列*/
	msg_id = msg_queue_create();
	msg_queue_delete(msg_id);
	msg_id = msg_queue_create();
	
	/*创建网络套接字*/
	listen_fd = sock_create(28888);
	
	arg_t arg = {
		.arg_1 = listen_fd,
		.arg_2 = msg_id,
	};
	
	/*创建并启动各模块线程*/
//	module_run(&arg, simul_env);                  	//用于调试,在没有硬件 	
													//设备时,开启此线程,	
													//产生随机的环境信息

//	module_run(&arg, simul_image);                  //用于调试,没有摄像头时, 
													//开启此线程,演示图片
	/*创建并启动各模块线程*/
	module_run(&msg_id, pthread_camera);			//图片采集
	module_run(&msg_id, pthread_transfer);			//m0数据采集
	module_run(&msg_id, pthread_image);				//发送图片到客户端
	module_run(&msg_id, pthread_web);				//web服务器
	module_run(&msg_id, pthread_uart_cmd);			//发送控制命令到m0
	module_run(&arg, pthread_wait);					//等待客户端连接
	module_run(&msg_id, pthread_sms);				//短信收发
	
	while(1) {
		sleep(8);
		//pint_env(env_msg);
		//printf("hi i'm alive\n");
	}
	
	return 0;
}
