#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <iconv.h>
#include <string.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "include/gsm.h"
#include "include/iconv.h"
#include "include/serial.h"
#include "include/msg.h"
#include "include/m0.h"
#include "include/serial.h"

#define DELAY_SEC	2

//#define DEBUG

int msq_id;
int serial_fd;
char gsm_buf[2048];

int change_sendphone_t(char phone[20])
{
	int i;
	int b;
	int len = strlen(phone);

	for(i=0; i<len; ) {
		b = phone[i];
		phone[i] = phone[i+1];
		phone[i+1] = b;
		i = i+2;
	}

	strcat(phone,"\0");
}

int change_time_t(char time[20])
{
	int i = 0,j = 0;
	int b;
	char d;
	int len = strlen(time);
	char string[len+20];
	bzero(string,sizeof(string));
	strncpy(&string[j++],&time[i++],1);
	strncpy(&string[j++],&time[i++],1);
	d = string[j-1];
	string[j-1] =string[j-2];
	string[j-2] = d; 
	strncpy(&string[j++],"-",1);
	strncpy(&string[j++],&time[i++],1);
	strncpy(&string[j++],&time[i++],1);
	d = string[j-1];
	string[j-1] =string[j-2];
	string[j-2] = d; 
	strncpy(&string[j++],"-",1);
	strncpy(&string[j++],&time[i++],1);
	strncpy(&string[j++],&time[i++],1);
	d = string[j-1];
	string[j-1] =string[j-2];
	string[j-2] = d; 
	strncpy(&string[j++]," ",1);
	strncpy(&string[j++],&time[i++],1);
	strncpy(&string[j++],&time[i++],1);
	d = string[j-1];
	string[j-1] =string[j-2];
	string[j-2] = d; 
	strncpy(&string[j++],":",1);
	strncpy(&string[j++],&time[i++],1);
	strncpy(&string[j++],&time[i++],1);
	d = string[j-1];
	string[j-1] =string[j-2];
	string[j-2] = d; 
	strncpy(&string[j++],":",1);
	strncpy(&string[j++],&time[i++],1);
	strncpy(&string[j++],&time[i++],1);
	d = string[j-1];
	string[j-1] =string[j-2];
	string[j-2] = d; 
	memset(time,0,sizeof(time));
	strcpy(time,string);
}

int change_recvphone_t(char phone[15])
{
	int i;
	int b;
	int len = strlen(phone);

	for(i=0; i<len; )
	{
		b = phone[i];
		phone[i] = phone[i+1];
		phone[i+1] = b;
		i = i+2;
	}
	strcpy(&phone[i-1], "\0");
}

int send_msg(int fd,send_MSG msg)
{
	char mid_buf[1024];
	char send_buf[1024];
	char recv_buf[1024];
	int n_write;
	int n_read;
/********************测试是否与串口联通****************************/
	//memset(send_buf, 0, sizeof(send_buf));
	//strcpy(send_buf, "AT\r");
	//gsm_gprs_send_cmd_read_result(fd, send_buf, DELAY_SEC);
	
	
/***********************模式选择********************************/
	//memset(send_buf, 0, sizeof(send_buf));
	//strcpy(send_buf, "AT+CMGF=0\r");
	//gsm_gprs_send_cmd_read_result(fd, send_buf, DELAY_SEC);
 
/*************************测试**** ****************************/
	//memset(send_buf, 0, sizeof(send_buf));
	//strcpy(send_buf, "AT+CPMS?\r");
	//gsm_gprs_send_cmd_read_result(fd, send_buf, DELAY_SEC);
	
/********************发送信息获取******************************/
	memset(send_buf, 0, sizeof(send_buf));
	char buf1[] = "0011000D91";
	char buf2[128];
	memset(buf2, 0, sizeof(buf2));
	strcpy(buf2, msg.phone);
	char buf3[] = "000801";
	char buf4[1024];
	memset(buf4, 0, sizeof(buf4));
	strcpy(buf4, msg.news);
	strcpy(send_buf, buf1);
	strcat(send_buf, buf2);
	strcat(send_buf, buf3);
	strcat(send_buf, buf4);
	memset(mid_buf, 0, sizeof(send_buf));
	strcpy(mid_buf, send_buf);
	
/*********************发送长度设置************************************/
	int m = (strlen(send_buf)/2) - 1;
	printf("发送长度：%d\n", m);
	char buf[1024];
	memset(buf, 0, sizeof(buf));
	sprintf(buf, "AT+CMGS=%d\r", m);
	gsm_gprs_send_cmd_read_result(fd, buf, DELAY_SEC);

/*******************发送***********************************/
	printf("发送 ...\n");
	strcpy(send_buf, mid_buf);
	strcat(send_buf, "\x1A");
	gsm_gprs_send_cmd_read_result(fd, send_buf, DELAY_SEC);

/**********************结束处理**************************/
	printf("结束发送 ...\n");
	//gsm_gprs_send_cmd_read_result(fd, "\x1A", DELAY_SEC);	
}

int gsm_msg_send(send_MSG msg, int str_len, int fd)
{
	puts(msg.news);
	strcat(msg.phone,"F");
	change_sendphone_t(msg.phone);
	puts(msg.phone);
	printf("%d\n", strlen(msg.phone));
	//puts(msg.news);
	char outbuf_temp[1024];
	char *outbuf = outbuf_temp;
	if(0 != iconv_fun_to_unicode(msg.news, &outbuf, str_len)) {
		perror("iconv_t");
	}
	bzero(msg.news, sizeof(msg.news));
	strcpy(msg.news, outbuf);
	/*发消息*/
	send_msg(fd, msg);
}

void init_ttyS(int fd)  
{  
    struct termios options;  
    bzero(&options, sizeof(options));       // clear options  
    cfsetispeed(&options,B9600);            // setup baud rate  
    cfsetospeed(&options,B9600);  
    options.c_cflag |= (CS8 | CREAD);  
    options.c_iflag = IGNPAR;  
    tcflush(fd, TCIFLUSH);  
    tcsetattr(fd, TCSANOW, &options);  
}

int read_gsm_gprs_datas(int fd, char *rcv_buf,int rcv_wait)
{  
    int retval;  
    fd_set rfds;  
    struct timeval tv;  
	char buf[2048] = {0};
  
    int ret = 0, pos = 0;  

    tv.tv_sec = rcv_wait;   	
    tv.tv_usec = 0;
    pos = 0; 					//point to rceeive buf  
      
    while (1) {  
		FD_ZERO(&rfds);  
		FD_SET(fd, &rfds);  
		retval = select(fd+1 , &rfds, NULL, NULL, &tv);  

		if (retval == -1) {
			perror("read_GSM_GPRS_datas timeout"); 	//超时  
			break;
		}  else if (retval) {
			//判断是否还有数据  
			ret = read(fd, buf + pos, 2048);
			pos += ret;     					//读出的数据长度  
			if (buf[pos-1] == '\r') {  //如果读到字符串末尾  
				FD_ZERO(&rfds);  
				FD_SET(fd, &rfds);  
				retval = select(fd+1 , &rfds, NULL, NULL, &tv);  
				if (!retval) break;     // no datas, break  
			}
	    } else {  
			printf("read_GSM_GPRS_datas(No data)\n");  
            break;
        }
	}
	
	strcpy(rcv_buf, buf);
	return 1;
}

int send_gsm_gprs_cmd(int fd, char *send_buf)
{
    int ret;
    ret = write(fd, send_buf, strlen(send_buf));
    if (ret == -1) {
			printf ("send_GSM_GPRS_cmd %s error\n", SMS_SERIAL);
			return -1;
    }

	return 1;
}

void gsm_gprs_send_cmd_read_result(int fd, char *send_buf, int rcv_wait)
{  
    char rcv_buf[2048] = {0};
    if((send_buf == NULL) || (send_gsm_gprs_cmd(fd, send_buf))) {   // send success , then read  
        if (read_gsm_gprs_datas(fd, rcv_buf, rcv_wait)) {
			printf ("%s\n", rcv_buf);
			bzero(gsm_buf, sizeof(gsm_buf));
			strcpy(gsm_buf, rcv_buf);       //gsm_buf 全局变量
		} else {
			printf ("GSM_GPRS_send_cmd_read_result(read error)\n");  
		}
    } else{
        printf("GSM_GPRS_send_cmd_read_result(send error)\n");  
    }
}

void gsm_read_message(int fd)
{
	//char *sm = "AT+CPMS=SM";
	//gsm_gprs_send_cmd_read_result(fd, sm, DELAY_SEC);
    //char *send_buf="AT+CMGL=4\r";         //列出存储的信息  
    char *send_buf = "AT+CMGR=1\r";
    gsm_gprs_send_cmd_read_result(fd, send_buf, DELAY_SEC);
	//char *confirm = "AT+CNMA\r";
	//gsm_gprs_send_cmd_read_result(fd, confirm, DELAY_SEC);
    //printf("end read all short message\n");
}

void gsm_simple_test(int fd)  
{  
    char *send_buf = "AT\r"; 
    gsm_gprs_send_cmd_read_result(fd, send_buf, DELAY_SEC);
	/*char *send_buf2 = "AT+CPMS?\r"; 
    gsm_gprs_send_cmd_read_result(fd, send_buf2, DELAY_SEC);*/
}

void gsm_del_message(int fd)
{
	/**
	删除所有短信，可以使用指定删除标识4(为删除全部)
	命令为AT+CMGD=1,4 
	设置命令：AT+CMGD=<index>[,<delflag>] 
	返回结果：ERROR/+或CME ERROR: <err> 失败 
	OK 成功
	测试命令 AT+CMGD=? - 
	参数的详细说明 
	<index> 1～255 整数型；关联存储器支持的地址编号范围内的取值
	<delflag>
	- 删除<index>指定的短信 
	0 删除<index>指定的短信 
	1 全部删除存储器中的已读短信 
	2 全部删除存储器中的已读和已发送短信 
	3 全部删除存储器中的已读、已发送和未发送短信 
	4 全部删除存储器中的已读、未读、已发送和未发送短信
	**/
	/*char *send_buf = "AT+CMGD=1\r";
	gsm_gprs_send_cmd_read_result(fd, send_buf, DELAY_SEC);
	if(NULL == strstr(gsm_buf, "OK")) {
		close(serial_fd);
		printf("close sms_run serial_fd = %d\n", serial_fd);
		sms_init();
	}*/
    char *send_buf1 = "AT+CMGD=1,2\r";         	//删除短信息
    gsm_gprs_send_cmd_read_result(fd, send_buf1, DELAY_SEC);
	if(NULL == strstr(gsm_buf, "OK")) {
		close(serial_fd);
		printf("close sms_run serial_fd = %d\n", serial_fd);
		sms_init();
	}
    //printf("delete all short message\n");
}

void gsm_conf_message(int fd)  
{  
	//设定SMS的控制方式,=0为UDP模式,=1为文本模式
    char *send_buf = "AT+CMGF=0\r";
    gsm_gprs_send_cmd_read_result(fd, send_buf, DELAY_SEC);

	//char *send_buf2 = "AT+CPMS?\r";
	//GSM_GPRS_send_cmd_read_result(fd, send_buf2, DELAY_SEC);
	gsm_del_message(fd);
	
	printf("end config short message env\n"); 
}

void cmd_handle(recv_MSG *msg)
{
	send_MSG send_msg;
	MSG m0_ctl_msg;
	char buf[1024] = {0};
	char *out = buf;
	bzero(&m0_ctl_msg, sizeof(m0_ctl_msg));
	//if(0 == strncmp(msg->phone, "15680489676", 11))
	if(strstr(gsm_buf, "5186409876")) {
		stpcpy(msg->phone, "15680489676");
		if(strstr(gsm_buf, "5F007167660E")) {
			iconv_fun_to_utf8("5F007167660E", &out);
			m0_ctl_msg.msg_type = UART_CMD_M0;
			strcpy(m0_ctl_msg.msg_data, "LED_ON");
			msg_queue_insert(msq_id, m0_ctl_msg);
		} else if(strstr(gsm_buf, "51737167660E")) {
			iconv_fun_to_utf8("51737167660E", &out);
			m0_ctl_msg.msg_type = UART_CMD_M0;
			strcpy(m0_ctl_msg.msg_data, "LED_OFF");
			msg_queue_insert(msq_id, m0_ctl_msg);
		} else if(strstr(gsm_buf, "5F0098CE6247")) {
			iconv_fun_to_utf8("5F0098CE6247", &out);
			m0_ctl_msg.msg_type = UART_CMD_M0;
			strcpy(m0_ctl_msg.msg_data, "FAN_ON");
			msg_queue_insert(msq_id, m0_ctl_msg);			
		} else if(strstr(gsm_buf, "517398CE6247")) {
			iconv_fun_to_utf8("517398CE6247", &out);
			m0_ctl_msg.msg_type = UART_CMD_M0;
			strcpy(m0_ctl_msg.msg_data, "FAN_OFF");
			msg_queue_insert(msq_id, m0_ctl_msg);
		} else if(strstr(gsm_buf, "83B753D673AF58834FE1606F")) {
			iconv_fun_to_utf8("83B753D673AF58834FE1606F", &out);
			bzero(msg->news,sizeof(msg->news));
			sprintf(msg->news, "温度:%d°C 湿度:%d%%RH 光强:%dLUX", \
						env_msg.tmp, env_msg.hum, env_msg.light);
			bzero(&send_msg, sizeof(send_msg));
			strcat(send_msg.phone,"86");
			strcat(send_msg.phone, msg->phone);
			strcpy(send_msg.news, msg->news);
			gsm_msg_send(send_msg, strlen(send_msg.news) - 2 * 6, serial_fd);
			sleep(1);
		} else if(strstr(gsm_buf, "517362A58B66")) {
			iconv_fun_to_utf8("517362A58B66", &out);
			m0_ctl_msg.msg_type = UART_CMD_M0;
			strcpy(m0_ctl_msg.msg_data, "BUZZ_OFF");
			msg_queue_insert(msq_id, m0_ctl_msg);			
		} else if(strstr(gsm_buf, "5F0062A58B66")) {
			iconv_fun_to_utf8("5F0062A58B66", &out);
			m0_ctl_msg.msg_type = UART_CMD_M0;
			strcpy(m0_ctl_msg.msg_data, "BUZZ_ON");
			msg_queue_insert(msq_id, m0_ctl_msg);
		} else {
			iconv_fun_to_utf8("5E2E52A9", &out);								/*获取help文档*/
			bzero(msg->news,sizeof(msg->news));
			strcpy(msg->news, "欢迎使用大棚监控系统\n请回复指令：1.开照明 2.关照明  3.开风扇 4.关风扇 5.开报警 6.关报警 7.获取环境信息");
			bzero(send_msg.phone, sizeof(send_msg.phone));
			strcat(send_msg.phone,"86");
			strcat(send_msg.phone, msg->phone);
			strcpy(send_msg.news, msg->news);
			gsm_msg_send(send_msg, strlen(send_msg.news) - 2 * 39, serial_fd);
			sleep(1);
		}
		printf("\n");
	}	
}

void msg_analysis(char *recv_buf)
{
	recv_MSG  msg;

/*************信息来源处理*********************/
	memset(msg.phone,0,sizeof(msg.phone));
	char *p = recv_buf;
	p = p + 52;
	if(strncmp(p,"68",2)!= 0);
	else
	{
			p = p+2;
	}
	memcpy(msg.phone,p,12);
	change_recvphone_t(msg.phone);

/****************信息时间处理*****************************/
	memset(msg.time,0,sizeof(msg.time));
	strncpy(msg.time,p+16,12);
	change_time_t(msg.time);	

/*****************信息内容处理*****************************/	
	if(strlen(recv_buf) >16) {
		memset(msg.news,0,sizeof(msg.news));
		strncpy(msg.news,p+32,strlen(recv_buf)-92);
	}
	else {
		printf("no message!\n");		
		return ;
	}

/**********************显示消息**********************************/
	printf("信息来源:\n");
	puts(msg.phone);
	printf("时间:\n");
	puts(msg.time);
	printf("信息:\n");
	puts(msg.news);

/******************** 控制命令解析******************************/
	cmd_handle(&msg);

/******************** 删除所有短信******************************/	
	gsm_del_message(serial_fd);
	sleep(1);
}

int sms_init()
{
	serial_fd = open_serial(SMS_SERIAL);
	if(serial_fd < 0) {
		printf("open sms_serial failed\n");
		return -1;
	}
	printf("sms_run serial_fd = %d\n", serial_fd);
	init_ttyS(serial_fd);
	gsm_conf_message(serial_fd);
	
	return serial_fd;
}

//int main()
int sms_run(void *arg)
{
	int ret = 0;
	ret = sms_init();
	if(ret == -1) {
		goto end;
	}
	msq_id = *(int *)arg;
	while(1) {
		bzero(gsm_buf, sizeof(gsm_buf));
		gsm_simple_test(serial_fd);
		sleep(2);
		if(strstr(gsm_buf, "OK")) {
			gsm_read_message(serial_fd);
			msg_analysis(gsm_buf);
		} else {
			close(serial_fd);
			printf("close sms_run serial_fd = %d\n", serial_fd);
			break;
		}
	}
	
end:
	return ret;
}