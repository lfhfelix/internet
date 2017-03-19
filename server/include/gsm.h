#ifndef __GSM_H__
#define __GSM_H__

#include "debug.h"

typedef struct
{
		char phone[15];
		char news[256];		
		
		
}send_MSG;

typedef struct
{
		char phone[15];
		char news[1024];
		char time[36];
}recv_MSG;

int change_sendphone_t(char phone[20]);
int change_time_t(char time[20]);
int change_recvphone_t(char phone[15]);
int send_msg(int fd, send_MSG msg);
int gsm_msg_send(send_MSG msg, int str_len, int fd);
int send_gsm_gprs_cmd(int fd, char *send_buf);
void gsm_simple_test(int fd);
void gsm_read_message(int fd);
int read_gsm_gprs_datas(int fd, char *rcv_buf,int rcv_wait);
int send_gsm_gprs_cmd(int fd, char *send_buf);
void msg_analysis(char *recv_buf);
void gsm_del_message(int fd);
void gsm_gprs_send_cmd_read_result(int fd, char *send_buf, int rcv_wait);
void gsm_conf_message(int fd);
void cmd_handle(recv_MSG *msg);
void init_ttyS(int fd);
int sms_run(void *arg);

#endif
