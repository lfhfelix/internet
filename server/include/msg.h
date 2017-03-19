#ifndef __MSG_H__
#define __MSG_H__

#include "debug.h"

#define REQ_QUIT		886				//REQ_QUIT|
#define REQ_LOGIN		1				//REQ_LOGIN|hello|123456

/*msg_type*/
#define REQ_IMAGE 		2				//GET_IMAGE|
#define IMG_OK 			3				//IMG_OK|
#define GET_ENV_M0		4				//获取M0采集到的环境信息GET_ENV_M0|
#define UART_CMD_M0		5				//发送控制命令到M0，UART_CMD_M0|led_on
#define CTL_GSM_SMS		6				//SMS模块控制，CTL_GSM_SMS|gsm_send
#define SET_ENV_LIMIT	7				//SET_ENV_LIMIT|TEPR_MAX|36

#define MAX_DATA_LEN	128


//服务器要发送给客户端的消息类型和消息内容
typedef struct {
	long msg_type;
	char msg_data[MAX_DATA_LEN];                     //密码或消息内容
} MSG;

/*创建消息队列，成功返回消息队列id，失败返回-1*/
int msg_queue_create();
/*插入消息到消息队列中,成功返回0，失败返回-1*/
int msg_queue_insert(int msg_id, MSG msg);
/*读取消息队列，成功返回0,失败返回-1*/
int msg_queue_recv(int msg_id, int msg_type, const MSG *msg);
/*删除消息队列*/
int msg_queue_delete(int msg_id);
/*消息封装*/
int msg_pack(MSG *msg, char *str, int len);

#endif