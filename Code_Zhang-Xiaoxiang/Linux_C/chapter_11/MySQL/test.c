// MySQL 练习1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>

int main(void)
{
    int ret;
    MYSQL_ROW row;
    MYSQL * mysql = NULL;           // 创建句柄
    mysql = mysql_init(mysql);      // 初始化
    if (mysql == NULL)
    {
        printf("init mysql failed.\n");
        exit(1);
    }
    char * sql = "select * from items;";
    mysql_real_connect(mysql, NULL, "root", "mysqlpass", "yiibaidb", 0, NULL, 0);   // 连接
    ret = mysql_real_query(mysql, sql, strlen(sql));
    if (ret)
    {
        mysql_error(mysql);
        mysql_close(mysql); 
        free(mysql);
        exit(1);
    }
    MYSQL_RES * res = mysql_store_result(mysql);
    while ((row = mysql_fetch_row(res)))
    {
        printf("%s %s\n", row[0], row[1]);
    }
    mysql_free_result(res);         // 释放结果集
    mysql_close(mysql);             // 关闭连接
}
