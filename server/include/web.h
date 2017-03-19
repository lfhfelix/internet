#ifndef __WEB_H__
#define __WEB_H__

#include "debug.h"

void *web_main(void *arg);
void http_process(int connfd);
void *pthread_web(void *arg);

#endif