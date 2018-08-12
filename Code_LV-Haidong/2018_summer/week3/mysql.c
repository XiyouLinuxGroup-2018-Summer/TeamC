#include<mysql/mysql.h>
#include<stdlib.h>
#include<stdio.h>

int main()
{
    MYSQL *conn;
    //步骤一:初始化句柄
    conn = mysql_init(NULL);

    if(conn == NULL)
    {
        printf("mysql_init failed!\n");
        return -1;
    }

    //步骤二：进行实际连接
    //参数分别为:conn连接句柄,host是MySQL所在的主机或者地址,user用户名,passwd密码,database_name数据库名,后面都是默认
    conn = mysql_real_connect(conn,"localhost","root","Lhd013012?","mysql",0,NULL,0);
    if(conn)
    {
        printf("Connection success!\n");
    }
    else
    {
        printf("Connection failed!\n");
    }
    mysql_close(conn);

    return 0;
}