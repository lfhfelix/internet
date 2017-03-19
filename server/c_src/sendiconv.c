#include <stdio.h>
#include <math.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <iconv.h>

#include "include/iconv.h"

#define OUTLEN 255

//#define DEBUG

int head_len(char * head, int a)
{
	char q[256];
	sprintf(head, "%x", a);
	if(a <16) {
		strcpy(q, "0");
		strcat(q, head);
		strcpy(head, q);
	}
	printf("code head : %s\n", head);
}

void invert_swap(char *swap, int len)
{
	int count = 0;
	printf("invert_swap len = %d\n", len);
	while(count < len) {
		if(0 == count % 2) {
			*(swap + count) = *(swap + count) ^ *(swap + count + 1);
			*(swap + count + 1) = *(swap + count) ^ *(swap + count + 1);
			*(swap + count) = *(swap + count) ^ *(swap + count + 1);
		}
		count ++;
	}
}

/*将utf-8转换为unicode*/
int iconv_fun_to_unicode(char *input, char **output, int str_len)
{
	size_t len = strlen(input);
	char buf[1024] = {0};
	char buf_temp[1024] = {0};
	char bit[8] = {0};
	char last_bit[8] = {1};
	char *pr = buf;
	int i = 0;
	
	covert("UNICODE", "UTF-8", input, len, *output, OUTLEN);
	
	*output = *output + 2;
	invert_swap(*output, 2 * str_len);
	while(i < 2 * str_len) {
		sprintf(bit, "%08x", *(*output + i));
		sprintf(pr, "%c%c", bit[6], bit[7]);
		if(bit[6] == '0' && bit[7] == '0' && \
			last_bit[6] == '0' && last_bit[7] == '0') {
			i --;
			break;
		}
		sprintf(last_bit, "%08x", *(*output + i));
		pr = pr + 2;
		i ++;
	}
	buf[2 * i] = '\0';
	len = strlen(buf) / 2;
	head_len(buf_temp, len);
	strcat(buf_temp, buf);
	strcpy(*output, buf_temp);
	
	printf("______HEAD__UNICODE_______\n");
	puts(*output);
	printf("______HEAD__UNICODE_______\n");
	
	return 0;	
}

int covert(char *tocode, char *fromcode, char *input, size_t ilen, char *output, size_t olen)
{
	char **pin = &input;
	char **pout = &output;
	iconv_t cd = iconv_open(tocode, fromcode);
	if((iconv_t)(-1) == cd) {
		return -1;			
	}
	
	memset(output,0,olen);
	if(iconv(cd, pin, &ilen, pout, &olen)) {	
		return -1;
	}
	
	iconv_close(cd);
	return 0;
}

int test_utf8_to_unicode()
{
	char buf[1024] = "ledon";
	char *input = buf;
	char output[1024];
	char *out = output;
	int str_len = strlen(input);
	iconv_fun_to_unicode(input, &out, str_len);
}

unsigned long strtox(char* str)
{
	unsigned long var=0;
	unsigned long t;
	int len = strlen(str);
	 
	if (var > 8) 					//最长8位
		return -1;
	for (; *str; str++)	{
		if (*str>='A' && *str <='F')
		t = *str-55;				//a-f之间的ascii与对应数值相差55如'A'为65,65-55即为A
		else
			t = *str-48;
			var<<=4;
			var|=t;
	}
	
	return var;
}

/*将unicode转换为utf-8*/
int iconv_fun_to_utf8(char *input, char **output)
{
	size_t len = strlen(input);
	char buf[1024] = {0};
	char nod[2];
	int i = 0;
	int count = 0;
	while(i < len) {
		if(i % 2 == 0) {
			count ++;
			sprintf(nod, "%c%c", *(input + i), *(input + i + 1));
			buf[count] = strtox(nod);
		}
		i ++;
	}
	i = 1;
	while(i <= count){
		printf("%2x ", buf[i]);
		i ++;
	}
	printf("\n");
	
	buf[count + 1] = '\0';
	invert_swap(buf + 1, count);
	covert("UTF-8", "UNICODE", buf + 1, count, *output, OUTLEN);
	
	printf("__________UTF-8____________\n");
	puts(*output);
	printf("__________UTF-8____________\n");
	
	return 0;	
}

int test_unicode_to_utf8()
{
	char buf[1024] = {0};
	char *output = buf;
	iconv_fun_to_utf8("7167660E5F00", &output);
}

#if 0
int main()
{
	//test_utf8_to_unicode();
	test_unicode_to_utf8();
}
#endif