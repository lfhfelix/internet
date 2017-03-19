#ifndef _M0_H_
#define _M0_H_

#include "debug.h"

#define ENV_BUF_LEN		36			//������Ϣ���ݰ����ȣ�bigzee

/*M0 CMD type*/
#define LED_ON		0x00		//led1 on
#define LED_OFF		0x01		//led1 off
#define BUZZ_ON		0x02		//buzz on
#define BUZZ_OFF	0x03		//buzz off
#define FAN_ON		0x04		//fan on
#define FAN_OFF		0x08		//fan off

#define FAN_LOW		0x05		//fan low
#define FAN_MID		0x06		//fan mid
#define FAN_HIGH	0x07		//fan high
#define SEG_ON		0x09		//shumaguan on
#define SEG_OFF		0x0A		//shumaguan off
#define M0_OFF		0x0B		//close banzi

/**
	0 << 0 ��ʶ���ȹ�
	1 << 1 //��ʶ�����ڵ��ٹ���
	1 << 2 //��ʶ�������ٹ���
	1 << 3 //��ʶ���ȸ��ٹ���
	**/
typedef struct {
	int id;					//�ڵ�id
	int tmp;				//�¶�
	int hum ;				//ʪ��
	int light;				//����ǿ��
	int led;				//����״̬
	int fan;				//����״̬
	int buzz;				//������״̬
	int tmp_max;			//����¶�
	int tmp_min;			//����¶�
	int hum_max;			//���ʪ��
	int hum_min;			//���ʪ��
} env_t;

env_t env_msg;

/*����m0,ѭ���ɼ�������Ϣ*/
int read_m0_run();
/*����M0��������*/
int uart_cmd_send(char id, char cmd, int serial_fd);
/*��ӡ������Ϣ*/
void pint_env(const env_t penv);
int uart_env_recv(int serial_fd, env_t *penv);

#endif