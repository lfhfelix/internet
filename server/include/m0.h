#ifndef _M0_H_
#define _M0_H_

#include "debug.h"

#define ENV_BUF_LEN		36			//环境信息数据包长度，bigzee

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
	0 << 0 标识风扇关
	1 << 1 //标识风扇在低速工作
	1 << 2 //标识风扇中速工作
	1 << 3 //标识风扇高速工作
	**/
typedef struct {
	int id;					//节点id
	int tmp;				//温度
	int hum ;				//湿度
	int light;				//光照强度
	int led;				//照明状态
	int fan;				//风扇状态
	int buzz;				//蜂鸣器状态
	int tmp_max;			//最高温度
	int tmp_min;			//最低温度
	int hum_max;			//最高湿度
	int hum_min;			//最低湿度
} env_t;

env_t env_msg;

/*启动m0,循环采集环境信息*/
int read_m0_run();
/*发送M0控制命令*/
int uart_cmd_send(char id, char cmd, int serial_fd);
/*打印环境信息*/
void pint_env(const env_t penv);
int uart_env_recv(int serial_fd, env_t *penv);

#endif