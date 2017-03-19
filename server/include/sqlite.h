#ifndef __SQLITE_H__
#define __SQLITE_H__

#include <sqlite3.h>

#include "debug.h"

#define MAX_LEN	128

//DB_T type
#define USER_LOGIN	11

#define USR_DB	"usr"		//用户信息表名

typedef struct {
	int type;                   //类型：用户，物品
	char name[MAX_LEN];         //用户名，物品名
	char text[MAX_LEN];			//用户密码，物品信息（数量）
} DB_T;

/*打开数据库，没有则自动创建，成_功返回创建的数据库句柄，失败返回NULL*/
sqlite3 *mydb_open();
/*关闭数据库，成功返回0，失败返回-1*/
int mydb_close(sqlite3 *my_db);
/*在指定的数据库中创建表，成功返回0，失败返回-1*/
int create_table(sqlite3 *my_db, char *table_name);
/*向指定的表中插入数据，成功返回0，失败返回-1*/
int sql_insert(sqlite3 *my_db, char *table, DB_T *data);
/*查询表中的数据，成功返回0，失败返回-1*/
int sql_search(sqlite3 *my_db, char *table, DB_T *data);
/*显示表中的内容，成功返回0，失败返回-1*/
int sql_display(sqlite3 *my_db, char *table);
/*回调函数，显示表中数据时被回调*/
int sql_callback(void *arg, int f_num, char **f_value, char **f_name);

#endif