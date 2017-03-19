#ifndef __SQLITE_H__
#define __SQLITE_H__

#include <sqlite3.h>

#include "debug.h"

#define MAX_LEN	128

//DB_T type
#define USER_LOGIN	11

#define USR_DB	"usr"		//�û���Ϣ����

typedef struct {
	int type;                   //���ͣ��û�����Ʒ
	char name[MAX_LEN];         //�û�������Ʒ��
	char text[MAX_LEN];			//�û����룬��Ʒ��Ϣ��������
} DB_T;

/*�����ݿ⣬û�����Զ���������_�����ش��������ݿ�����ʧ�ܷ���NULL*/
sqlite3 *mydb_open();
/*�ر����ݿ⣬�ɹ�����0��ʧ�ܷ���-1*/
int mydb_close(sqlite3 *my_db);
/*��ָ�������ݿ��д������ɹ�����0��ʧ�ܷ���-1*/
int create_table(sqlite3 *my_db, char *table_name);
/*��ָ���ı��в������ݣ��ɹ�����0��ʧ�ܷ���-1*/
int sql_insert(sqlite3 *my_db, char *table, DB_T *data);
/*��ѯ���е����ݣ��ɹ�����0��ʧ�ܷ���-1*/
int sql_search(sqlite3 *my_db, char *table, DB_T *data);
/*��ʾ���е����ݣ��ɹ�����0��ʧ�ܷ���-1*/
int sql_display(sqlite3 *my_db, char *table);
/*�ص���������ʾ��������ʱ���ص�*/
int sql_callback(void *arg, int f_num, char **f_value, char **f_name);

#endif