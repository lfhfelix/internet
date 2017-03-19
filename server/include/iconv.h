#ifndef __ICONV_H__
#define __ICONV_H__

#include "debug.h"

int covert(char *desc, char *src, char *input, size_t ilen, char *output, size_t	olen);
int head_len(char * head, int a);
int iconv_fun(char *input, char **output);
int covert(char *tocode, char *fromcode, char *input, size_t ilen, char *output,size_t olen);
int test_unicode_to_utf8();

#endif
