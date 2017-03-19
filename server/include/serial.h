#ifndef _SERIAL8_H_
#define _SERIAL8_H_

#include "debug.h"

/*打开串口(该程序devpath = "/dev/ttyUSB0")，失败返回-1，成功返回被打开串口的文件描述符*/
int open_serial(const char *dev_path);
/*初始化串口，该程序参数(serial_fd, 115200, 8, 'N', 1)*/
int init_serial(int serial_fd, int nSpeed, int nBits, char nEvent, int nStop);
/*从串口读数据，返回读到的数据长度*/
int serial_read(int serial_fd, char *buf, int count);
/*写数据到串口，返回读到的数据长度*/
int serial_write(int serial_fd, char *buf, int count);
/*获取环境信息，保存到env_t数据结构中*/

#endif
