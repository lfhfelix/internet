#ifndef __ENTRY_H__
#define __ENTRY_H__

#include "socket.h"
#include "sqlite.h"
#include "msg.h"
#include "sqlite.h"
#include "modules.h"
#include "web.h"
#include "serial.h"
#include "m0.h"

/*GSM*/
#include "iconv.h"
#include "gsm.h"

#include "debug.h"

/*客户端请求响应*/
void *pthread_requst(void *p);
/*客户端退出*/
void client_login(int connect_fd, const char *user_msg);
/*客户端退出*/
void client_quit(int connect_fd);

#endif