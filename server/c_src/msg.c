#include <unistd.h>  
#include <stdlib.h>  
#include <stdio.h>  
#include <string.h>  
#include <errno.h>  

#include <sys/msg.h> 
#include <sys/types.h>
#include <sys/ipc.h>

#include "include/msg.h"

//#define DEBUG

/*创建消息队列，成功返回消息队列id，失败返回-1*/
int msg_queue_create()
{
	int msg_id;
	key_t key;
	
	if(-1 == (key = ftok(".", 1))) {
		perror("msg->key");
		return -1;
	}
	
	/*创建消息队列*/
	if(-1 == (msg_id = msgget(key, IPC_CREAT|666))) {
		perror("msg->key");
		return -1;
	}

#ifdef DEBUG
	printf("Open messege queue success (msd_id = %d)\n", msg_id);
#endif

	return msg_id;
}

/*插入消息到消息队列中,成功返回0，失败返回-1*/
int msg_queue_insert(int msg_id, MSG msg)
{	
	if(-1 == msgsnd(msg_id, (void*)&msg, MAX_DATA_LEN, 0)) {
        perror("msg->snd");
        return -1;
    }

#ifdef DEBUG
	printf("Insert msg_data : %s\n", msg.msg_data);
#endif

	return 0;
}

/*读取消息队列，成功返回0,失败返回-1*/
int msg_queue_recv(int msg_id, int msg_type, const MSG *msg)
{
	if(-1 == msgrcv(msg_id, (void*)msg, sizeof(msg->msg_data), msg_type, 0)) {  
		perror("msg->rcv");
		return -1;
    }
	

#ifdef DEBUG
	printf("Receive msg_data : %s\n", (*msg).msg_data);
#endif

	return 0;
}

/*删除消息队列*/
int msg_queue_delete(int msg_id)
{
	if(-1 == msgctl(msg_id, IPC_RMID, NULL))  
    {  
        perror("msg->delete");
        return -1;  
    }
	
	return 0;
}

/*消息封装*/
int msg_pack(MSG *msg, char *str, int len)
{
	char *pos;
	pos = strstr(str,"|");
	
	if(NULL == pos) {
		printf("msg->formot : error\n");
		return -1;
	} else {
		strncpy((char *)&msg->msg_type, str, pos - str);
		msg->msg_type = atoi((char *)&msg->msg_type);
		strcpy(msg->msg_data, pos + 1);
	}

#ifdef DEBUG
	printf("%ld\n", msg->msg_type);
	printf("%s\n", msg->msg_data);
#endif

	return 0;
}


/*测试代码0*/
#if 0
void main()
{
	//MSG msg;
	//memset(&msg, 0, sizeof(msg));
	//msg_pack(&msg, "1|user|123456", 8);
}
#endif

/*测试代码1*/
#if 0
void main() {
	int msg_id;
	MSG msg = {
		.msg_type = GET_IMAGE,
		.msg_data = "hello world!",
	};
	MSG recv_msg;
	
	msg_id = msg_queue_create();
	//msg_queue_insert(msg_id, msg);
	msg_queue_recv(msg_id, GET_IMAGE, &recv_msg);
}
#endif