#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "include/socket.h"

//#define DEBUG

/*创建套接字，成功返回套接字的文件描述符，失败返回-1*/
int sock_create(int port)
{
	int ret;
	int opt = 1;
	int listen_fd;
	struct sockaddr_in servaddr;
	
	listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == listen_fd) {
		perror("server->socket");
		return -1;
	}
	setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	
	memset(&servaddr, 0, sizeof(struct sockaddr_in));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	ret = bind(listen_fd, (struct sockaddr *)&servaddr, sizeof(struct sockaddr_in));
	if(-1 == ret) {
		perror("server->bind");
		close(listen_fd);
		return -1;
	}
	
	ret = listen(listen_fd, MAX_BACKLOG);
	if(-1 == ret) {
		perror("server->listen");
		close(listen_fd);
		return -1;
	}
	
	printf("Start listen...\n");
	
	return listen_fd;
}

/*建立套接字连接，成功返回建立好连接的套接字描述符，失败返回-1*/
int sock_getconfd(int port, int listen_fd)
{
	int addrlen;
	int connect_fd;
	struct sockaddr_in cliaddr;
	
	addrlen = sizeof(struct sockaddr_in);
	connect_fd = accept(listen_fd, (struct sockaddr *)&cliaddr, &addrlen);
	if(-1 == connect_fd) {
		perror("server->accept");
		//close(listen_fd);
		return -1;
	}

//#ifdef DEBUG
	printf("sock->accept:a client from %s is connected!\n", inet_ntoa(cliaddr.sin_addr));
//#endif

	return connect_fd;
}

/*接收客户端发送的数据，成功返回收到的字节数，失败返回-1，连接超时返回0*/
int sock_recv(int connect_fd, char *msgbuf, void *dey_sec)
{
	int ret = 0;
	char request[RECV_MAX_LEN];
	fd_set fds;
	struct timeval timeout;
	
	if(connect_fd < 0) {
		return -1;
	}
	
	FD_ZERO(&fds);
	FD_SET(connect_fd, &fds);
	
	if(dey_sec != NULL) {
		timeout.tv_sec = *(int *)dey_sec;
		timeout.tv_usec = 0;
		ret = select(connect_fd + 1, &fds, NULL, NULL, &timeout);
	}
	else 
		ret = select(connect_fd + 1, &fds, NULL, NULL, NULL);
	if (ret == -1) {
		perror("sock_recv");
		close(connect_fd);
		return -1;
	} else if (ret == 0) {
		printf("sock_recv timeout\n");
		return 0;
	} else if(FD_ISSET(connect_fd, &fds)) {
		ret = recv(connect_fd, request, sizeof(request), 0);
		if(ret <= 0) {
			if(0 == ret) {
				printf("sock->recv:end of file\n");
				close(connect_fd);
				printf("client quit\n");
				return ret;
			} else {
				perror("sock->recv");
				close(connect_fd);
				printf("client quit\n");
				return -1;
			}
		}
	}
	
	strncpy(msgbuf, request, ret);

#ifdef DEBUG
	printf("recv(%d bytes) : %s\n", ret, msgbuf);
#endif

	return ret;
}

/*发送数据到客户端，成功返回发送的字节数，失败返回-1，超时返回0*/
int sock_send(int connect_fd, const char *msgbuf, int send_len)
{
	int ret = 0;
	fd_set fds;
	struct timeval timeout;

	if(connect_fd < 0) {
		return -1;
	}
	
	timeout.tv_sec = 3;
	timeout.tv_usec = 0;
	FD_ZERO(&fds);
	FD_SET(connect_fd, &fds);
	
	ret = select(connect_fd + 1, NULL, &fds, NULL, &timeout);
	if (ret == -1) {
		perror("sock_send");
		return -1;
	} else if (ret == 0) {
		printf("sock_send timeout\n");
		return 0;
	} else if(FD_ISSET(connect_fd, &fds)) {
		ret = send(connect_fd, msgbuf, send_len, 0);
		if(-1 == ret) {
			perror("server->write");
			close(connect_fd);
			printf("client quit\n");
			return -1;
		}
	}

#ifdef DEBUG
	printf("send(%d bytes) : %s\n", ret, msgbuf);
#endif

	return ret;
}


/*测试代码*/
#if 0
int main()
{
	int listen_fd;
	int connect_fd;
	int recv_len = 0;
	char msgbuf[RECV_MAX_LEN];
	
	memset(msgbuf, 0, RECV_MAX_LEN);   //不加这句，通过套接字收发的信息，会自动在后面添加时间戳
	
	listen_fd = sock_create(28888);
	connect_fd = sock_getconfd(28888, listen_fd);
	recv_len = sock_recv(connect_fd, msgbuf);
	if(recv_len <= SEND_MAX_LEN)
		sock_send(connect_fd, msgbuf, recv_len);
}
#endif