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
    // printf("mysql pointer : %p\n", mysql);
    if (mysql == NULL)
    {
        printf("init mysql failed.\n");
        exit(1);
    }
    char * sql = "desc group_list;";
    mysql_real_connect(mysql, NULL, "root", "mysqlpass", "ChatR_Base", 0, NULL, 0);   // 连接
    ret = mysql_real_query(mysql, sql, strlen(sql));
    if (ret)
    {
        printf("%d >\n", __LINE__);
        mysql_error(mysql);
        mysql_close(mysql); 
        free(mysql);
        exit(1);
    }
    MYSQL_RES * res = mysql_store_result(mysql);

    // mysql_num_fields()取得字段的数目，
    // mysql_num_rows()取得行的数目！
    while ((row = mysql_fetch_row(res)))
    {
        for (int i = 0; i < mysql_num_fields(res); i++)
            printf("%-13s", row[i]);
        printf("\n");
    }
    
    // char str[100] = "show databases;";
    // ret = mysql_real_query(mysql, str, strlen(sql));
    // if (ret)
    // {
    //     printf("%d >\n", __LINE__);
    //     mysql_error(mysql);
    //     mysql_close(mysql); 
    //     exit(1);
    // }

    // mysql_free_result(res);         // 释放结果集
    mysql_close(mysql);             // 关闭连接
    // printf("mysql pointer : %p\n", mysql);
    // free(mysql);
    mysql = NULL;
}
