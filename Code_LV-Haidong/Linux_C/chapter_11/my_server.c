#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<stdio.h>
#include<string.h>
#include<netinet/ether.h>
#include <arpa/inet.h>
#include<errno.h>
#include "my_recv.h"

#define SERV_PORT 4507 //服务器的端口
#define LISTENQ 12 //连接请求的队列最大长度

#define INVALID_USERINFO 'n'  //用户信息无效
#define VALID_USERINFO 'y' //用户信息有效

#define USERNAME 0 //接受的是用户名
#define PASSWORD 1 //接受的是密码

struct userinfo {//保存用户名和密码的结构体
   char username[32];
   char password[32];
};

struct userinfo users[] = 
{
    {"linux","unix"},
    {"4507","4508"},
    {"clh","clh"},
    {"xl","xl"},
    {" "," "}//以只含有一个空格的字符串作为数组的结束标记
};
//查找用户名是不是存在，存在返回该用户名的下标,不存在则返回iu-1,出错返回-2
int find_name(const char *name)
{
    int i;
    if(name == NULL)
    {
        printf("")
    }
}













