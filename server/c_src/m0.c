#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/serial.h"
#include "include/msg.h"
#include "include/m0.h"

//#define DEBUG

/*获取环境信息*/
int uart_env_recv(int serial_fd, env_t *penv)
{
	char buf[36] = {0};
	int ret;
	
	ret = serial_read(serial_fd, buf, sizeof(buf));
	if(ret < 0) {
		printf("serial->read : error");
		return -1;
	}

	penv->id = (int)buf[1];
	penv->tmp = (int) (buf[5]);
	penv->hum = (int)buf[7];
	penv->light = (int) ( buf[20] + (buf[21]<< 8) + (buf[22] << 16) +(buf[23] << 24));
	penv->led = (int)buf[24];
	penv->fan = (int)buf[25];
	penv->buzz = (int)buf[26];
	
    if(buf[2] != 0x24) {
        printf("error data\n");
        return 0;
    }

	return (int)buf[2];
}

/**
	0xAA //开机
	0xBB //数据采集
	0xCC //rfid 信息
	0xDD //命令
	0xEE //按键
	0x00 //其他(未定义)
	**/
/*发送M0控制命令*/
int uart_cmd_send(char id, char cmd, int serial_fd)
{
	char buf[ENV_BUF_LEN] = {0};
	buf[0] = 0xdd;
	buf[1] = id;							//子节点id					
	buf[2] = 0x24;							//有效数据长度
	buf[3] = 0x04;							//对齐
	/*有效数据4~27位*/
	/*32~33 crc*/
	/*33~35对齐*/
	
	if(cmd >= 0x00 && cmd <=0x0B) {
		buf[4] = cmd;
	} else {
		printf("cmd->type: error\n");
		return -1;
	}
	
	buf[4] = cmd;
	serial_write(serial_fd, buf, sizeof(buf));
	return 0;
}

/*打印环境信息*/
void pint_env(const env_t penv)
{
	printf("ID : %d\n", penv.id);
	printf("TMP : %d\n", penv.tmp);
	printf("HUM : %d\n", penv.hum);
	printf("LIGHT : %d\n", penv.light);
	printf("LED : %d\n", penv.led);
	printf("FAN : %d\n", penv.fan);
	printf("BUZZ : %d\n", penv.buzz);
    puts("\n");
}

/*启动m0,循环采集环境信息*/
/*int write_m0_run(char cmd)
{
	int serial_fd;
	int ret = 0;
	//MSG *msg;

	serial_fd = open_serial(M0_SERIAL);
	if(serial_fd < 0)
	{
		printf("open serial_fd failed\n");
		return -1;
	}
	
	printf("serial_fd = %d\n", serial_fd);
	init_serial(serial_fd, 115200, 8, 'N', 1);
	
	fd_set rfds, wfds;
	//struct timeval timeout;
	while(1) {
		//ret = msg_queue_recv(msg_id, UART_CMD_M0, msg);
		if(ret < 0) {
			break;
		}
		FD_ZERO(&wfds);
		FD_SET(serial_fd, &wfds);
	
		ret = select(serial_fd + 1, NULL, &wfds, NULL, NULL);
		if (FD_ISSET(serial_fd, &wfds)) {
			ret = uart_cmd_send(serial_fd, SEND_M0_ID, 0x04);
			if(ret <= 0) {
				goto end;
			}
		}
	}

end:
    printf("close serial_fd : %d\n", serial_fd);
	close(serial_fd);
	return serial_fd;
}*/

/*启动m0,等待客户端控制*/
int read_m0_run()
{
	int ret = 0;
	env_t env_temp;
	int serial_fd;
	
	serial_fd = open_serial(M0_SERIAL);
	if(serial_fd < 0)
	{
		printf("open read_m0 serial failed\n");
		return -1;
	}
	
	printf("read_m0_run serial_fd = %d\n", serial_fd);
	init_serial(serial_fd, 115200, 8, 'N', 1);
	
	fd_set rfds, wfds;
	//struct timeval timeout;
	while(1) {
		FD_ZERO(&rfds);
		FD_SET(serial_fd, &rfds);
	
		ret = select(serial_fd + 1, &rfds, NULL, NULL, NULL);
		if (FD_ISSET(serial_fd, &rfds)) {
			memset(&env_temp, 0, sizeof(env_temp));
			ret = uart_env_recv(serial_fd, &env_temp);
			if(ret <= 0) {
				goto end;
			}
			
			if(env_temp.tmp != 0 && env_temp.hum != 0 && env_temp.light > 0 && M0_RECV_SELCET) {
				memcpy(&env_msg, &env_temp, sizeof(env_temp));

#ifdef DEBUG
				pint_env(env_msg);
#endif
			}
		}
	}

end:
    printf("close serial_fd : %d\n", serial_fd);
	close(serial_fd);
	return serial_fd;
}

/*测试代码*/
#if 0
int main()
{
	int fd;

	fd = open_serial(M0_SERIAL);
	if(fd < 0)
	{
		printf("open serial_fd failed\n");
		return -1;
	}
	
	printf("serial_fd = %d\n", fd);
	init_serial(fd, 115200, 8, 'N', 1);
			
	char buf_d[36] = {0xdd, 0x05, 0x24, 0x04, 0x08};
	char buf[36];
	//while(1)
	{
		uart_cmd_send(fd, 0x06, 0x04);
		sleep(4);
		uart_cmd_send(fd, 0x06, 0x08);
		//serial_read(fd, buf, 36);
	}
	
	return 0;
}
#endif