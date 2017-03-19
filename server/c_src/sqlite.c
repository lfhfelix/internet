#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "include/sqlite.h"

//#define	DEBUG

/*打开数据库，没有则自动创建，成功返回创建的数据库句柄，失败返回NULL*/
sqlite3 *mydb_open()
{
	int ret;
	char file_name[32] = "./my.db";
	sqlite3 *my_db = NULL;
	ret = sqlite3_open(file_name, &my_db);
    if(ret != SQLITE_OK) {
		printf("sqlite3_open error:%s\n", sqlite3_errmsg(my_db));
		return NULL;
	}

	return my_db;
}

/*关闭数据库，成功返回0，失败返回-1*/
int mydb_close(sqlite3 *my_db)
{
	if(my_db != NULL) {
		sqlite3_close(my_db);
		my_db = NULL;
		return 0;
	} else return -1;
}

/*在指定的数据库中创建表，成功返回0，失败返回-1*/
int create_table(sqlite3 *my_db, char *table_name)
{
	char sqlstr[128];
	char *errmsg;

	sprintf(sqlstr, "create table '%s' (name text primary key,data text)", table_name);
	if(SQLITE_OK != sqlite3_exec(my_db, sqlstr, NULL, NULL, &errmsg)) {
		printf("table->create:%s\n", errmsg);
		if(NULL != strstr(errmsg, "already exists")) {
			sqlite3_free(errmsg);
			return 0;
		} else {
			sqlite3_free(errmsg);
			return -1;
		}
	}

	return 0;
}

/*向指定的表中插入数据，成功返回0，失败返回-1*/
int sql_insert(sqlite3 *my_db, char *table, DB_T *data)
{
	char *errmsg;
	char sqlstr[128];
	
	sprintf(sqlstr, "insert into '%s' values('%s','%s')", table, data->name, data->text);
	if(SQLITE_OK != sqlite3_exec(my_db, sqlstr, NULL, NULL, &errmsg)) {
		printf("record->insert:%s or user already exists\n", errmsg);
		sqlite3_free(errmsg);
		return -1;
	}

	return 0;
}

/*查询表中的数据，成功返回0，失败返回-1*/
int sql_search(sqlite3 *my_db, char *table, DB_T *data)
{
	char sqlstr[128];
	char *errmsg;
	char **result;
	int nrow, ncolumn;

	if(strlen(data->name) < 1 || strlen(data->text) < 1) {
		return;
	}
	
	sprintf(sqlstr, "select * from '%s' where name = '%s' and data = '%s'", table, data->name, data->text);
	if(SQLITE_OK != sqlite3_get_table(my_db, sqlstr, &result, &nrow, &ncolumn, &errmsg)) {
		printf("Error:%s\n", errmsg);
		sqlite3_free(errmsg);
		sqlite3_free_table(result);
		return -1;
	}

	if(0 == nrow) {
		strcpy(data->text, "name or messenge is wrong");
		sqlite3_free_table(result);
		return -1;
	} else {
		memset(data->text, 0, sizeof(data->text));
		strcpy(data->text, "ok");
	}

#ifdef DEBUG
	printf("table : %s\n", table);
	printf("%s | %s\n", result[0], result[1]);
	printf("%s | %s\n", result[nrow*ncolumn], result[nrow*ncolumn + 1]);
#endif

	sqlite3_free_table(result);

	return 0;
}

/*回调函数，显示表中数据时被回调*/
int dis_callback(void *arg, int f_num, char **f_value, char **f_name)
{
	DB_T data;
	char str_buf[MAX_LEN];
	sprintf(str_buf, "%d | %s | %s", f_num, f_value[0], f_value[1]);
	printf("%s\n", str_buf);
	
	return 0;
}

/*显示表中的内容，成功返回0，失败返回-1*/
int sql_display(sqlite3 *my_db, char *table)
{
	char sqlstr[128];
	char *errmsg;
	
	sprintf(sqlstr, "select * from '%s'", table);
	printf("table %s list\n", table);
	if(SQLITE_OK != sqlite3_exec(my_db, sqlstr, dis_callback, NULL, &errmsg)) {
		printf("Error:%s\n", errmsg);
		sqlite3_free(errmsg);
		return -1;
	}

	return 0;
}


#if 0
int main()
{
	mydb_open();
	
	return 0;
}
#endif
