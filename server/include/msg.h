#ifndef __MSG_H__
#define __MSG_H__

#include "debug.h"

#define REQ_QUIT		886				//REQ_QUIT|
#define REQ_LOGIN		1				//REQ_LOGIN|hello|123456

/*msg_type*/
#define REQ_IMAGE 		2				//GET_IMAGE|
#define IMG_OK 			3				//IMG_OK|
#define GET_ENV_M0		4				//��ȡM0�ɼ����Ļ�����ϢGET_ENV_M0|
#define UART_CMD_M0		5				//���Ϳ������M0��UART_CMD_M0|led_on
#define CTL_GSM_SMS		6				//SMSģ����ƣ�CTL_GSM_SMS|gsm_send
#define SET_ENV_LIMIT	7				//SET_ENV_LIMIT|TEPR_MAX|36

#define MAX_DATA_LEN	128


//������Ҫ���͸��ͻ��˵���Ϣ���ͺ���Ϣ����
typedef struct {
	long msg_type;
	char msg_data[MAX_DATA_LEN];                     //�������Ϣ����
} MSG;

/*������Ϣ���У��ɹ�������Ϣ����id��ʧ�ܷ���-1*/
int msg_queue_create();
/*������Ϣ����Ϣ������,�ɹ�����0��ʧ�ܷ���-1*/
int msg_queue_insert(int msg_id, MSG msg);
/*��ȡ��Ϣ���У��ɹ�����0,ʧ�ܷ���-1*/
int msg_queue_recv(int msg_id, int msg_type, const MSG *msg);
/*ɾ����Ϣ����*/
int msg_queue_delete(int msg_id);
/*��Ϣ��װ*/
int msg_pack(MSG *msg, char *str, int len);

#endif