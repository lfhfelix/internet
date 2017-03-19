#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <termios.h>
#include <sys/types.h>

#include "include/serial.h"
#include "include/msg.h"

//#define DEBUG

/*打开串口(该程序devpath = "/dev/ttyUSB1")，失败返回-1，成功返回被打开串口的文件描述符*/
int open_serial(const char *dev_path)
{
	int serial_fd;
	
	//O_NOCTTY:程序不成为这个端口上的"控制终端"
	//serial_fd = open(dev_path, O_RDWR | O_NOCTTY | O_NDELAY);
	serial_fd = open(dev_path, O_RDWR | O_NOCTTY);
	if(0 > serial_fd) {
		perror("serial->open");
		return -1;
	}
	
	if(isatty(serial_fd) == 0) {
		printf("serial is not a terminal device \n");
	}
			
	//fcntl(serial_fd, F_SETFL, FNDELAY);		//FNDELAY：非阻塞
	//在不能得到数据的情况下,系统调用就一直等着
	if(fcntl(serial_fd, F_SETFL, 0) < 0) {
		printf("fcntl failed!\n");			
	}
	
#ifdef DEBUG
	printf("serial->open : ok\n");
#endif
	
	return	serial_fd;
}

/*串口初始化*/
int init_serial(int serial_fd, int nSpeed, int nBits, char nEvent, int nStop)  
{
	struct termios options;
	
	if(0 != tcgetattr(serial_fd,&options)) {
		perror("serial->set:failed\n");
		return -1;
	}
	
	//本地设置 
	options.c_cflag|=CLOCAL;
	options.c_cflag|=CREAD; 				//接受使能
	options.c_cflag&=~CSIZE; 				//字符长度控制
	options.c_cflag&=~CRTSCTS;				//无硬件流控
	options.c_oflag=0;						//输出模式  
	options.c_lflag=0;						//不激活终端模式   
	options.c_cflag&=~OPOST;
	options.c_cc[VTIME]=1;
	options.c_cc[VMIN]=1;					//1个字符
	tcflush(serial_fd,TCIFLUSH);
	
	/*设置数据位长度*/
	switch( nBits )	{
	case 5:
        options.c_cflag|=CS5;
        break;
	case 6:
        options.c_cflag|=CS6;
        break;
    case 7:
        options.c_cflag|=CS7;
        break;
    case 8:
        options.c_cflag|=CS8;
        break;
	default:
		printf("nBits->set : failed or usage error\n");
		return -1;
    }
	
	/*设置校验方式*/
	switch( nEvent ) {	
    case 'O':                     					//奇校验:Odd parity
        options.c_cflag |= PARENB;
        options.c_cflag |= PARODD;
        options.c_iflag |= (INPCK | ISTRIP);
        break;
    case 'E':                     					//偶校验:Even parity 
        options.c_cflag |= PARENB;
        options.c_cflag &= ~PARODD;
		options.c_iflag |= (INPCK | ISTRIP);
        break;
    case 'N':                    					//无校验
        options.c_cflag &= ~PARENB;
		options.c_cflag &= ~CSTOPB;
        break;
	default:
		printf("set nEvent fail\n");
		return -1;
    }
	
	/*设置波特率*/
	switch( nSpeed ) {
	case 2400:
        cfsetispeed(&options, B2400);
        cfsetospeed(&options, B2400);
        break;
    case 4800:
        cfsetispeed(&options, B4800);
        cfsetospeed(&options, B4800);
        break;
    case 9600:
        cfsetispeed(&options, B9600);
        cfsetospeed(&options, B9600);
        break;
    case 19200:
        cfsetispeed(&options, B19200);
        cfsetospeed(&options, B19200);
        break;    
    case 115200:
        cfsetispeed(&options, B115200);
        cfsetospeed(&options, B115200);
        break;
    default:
        printf("set nSpeed fail\n");
		return -1;
    }
	
	switch( nStop ) {
	case 1:
		options.c_cflag &=  ~CSTOPB;
		break;
	case 2:
		options.c_cflag |=  CSTOPB;
		break;
	default:
		printf("set nStop fail\n");
		return -1;
	}
	
	//常量TCSANOW标志所有改变必须立刻生效而不用等到数据传输结束
	if(tcsetattr(serial_fd,TCSANOW,&options)!=0) {
		perror("serial set error!\n");
		return -1;
	}

#ifdef DEBUG
	printf("serial->init : succeed\n");
#endif
	
	return 0;
}

/*从串口读数据，返回读到的数据长度*/
int serial_read(int serial_fd, char *buf, int count)
{
	int lenth;
	int ret = 0;
    char buf_temp[count];

	lenth = 0;

	while(lenth < count) {
		ret = read(serial_fd, buf_temp + lenth, count - lenth);
		if (0 > ret) {
			perror("serial->read");
			break;
		} else {
			lenth += ret;
		}
	}
    memcpy(buf, buf_temp, count);

#ifdef DEBUG	
	int i = 0;
	while(i < count) {
		printf("%x  ", buf_temp[i]);
		i ++;
	}
	printf("\n");
    printf("serial->read: %d bytes\n", lenth);
#endif

	return lenth;
}

/**/
int serial_write(int serial_fd, char *buf, int count)
{
	int lenth;
	int ret = 0;

	lenth = 0;

	while(lenth < count) {
		if (0 > (ret = write(serial_fd, buf + lenth, count - lenth))) {
			perror("serial->write");
			return -1;
		} else {
			lenth += ret;
		}
	}

//#ifdef DEBUG
	printf("serial->write(%d bytes) : ", lenth);
	int i = 0;
	while(i < count) {
		printf("%x  ", buf[i]);
		i ++;
	}
	printf("\n");
//#endif
	
	return lenth;
}