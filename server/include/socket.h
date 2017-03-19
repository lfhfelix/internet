#ifndef __SOCKET_H__
#define __SOCKET_H__

#include "debug.h"

#define MAX_BACKLOG 32
#define RECV_MAX_LEN (1024*20)
#define SEND_MAX_LEN (1024*20)

/*创建套接字，绑定端口号，设置监听，成功返回套接字的文件描述符，失败返回-1*/
int sock_create(int port);
/*建立套接字连接，成功返回建立好连接的套接字描述符，失败返回-1*/
int sock_getconfd(int port, int listen_fd);
/*接收客户端发送的数据，成功返回收到的字节数，失败返回-1，连接超时返回0*/
int sock_recv(int connect_fd, char *msgbuf, void *dey_sec);
/*发送数据到客户端，成功返回发送的字节数，失败返回-1*/
int sock_send(int connect_fd, const char *msgbuf, int send_len);

#endif