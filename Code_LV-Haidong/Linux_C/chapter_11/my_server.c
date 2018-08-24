#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<stdio.h>
#include<string.h>
#include<netinet/ether.h>
#include <arpa/inet.h>
#include<errno.h>
#include "my_recv.h"
#include<stdlib.h>
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
        printf("in find_name,NULL pointer");
        return -2;
    }
    for(i=0;users[i].username[0]!=' ';i++)
    {
        if(strcmp(users[i].username,name) == 0)
        {
            return i;
        }
    }
    return -1;
}
//发送数据
void send_data(int conn_fd,const char * string)
{
    if(send(conn_fd,string,strlen(string),0) < 0)
    {
        perror("send");
    }
}

int main()
{
    int sock_fd,conn_fd;
    int optval;
    int flag_recv = USERNAME;//标识接受到的是用户名和密码
    int ret;
    int name_num;
    pid_t pid;
    socklen_t cli_len;
    struct sockaddr_in cli_addr,serv_addr;
    char recv_buf[128];

    //创建一个TCP套接字
    sock_fd = socket(AF_INET,SOCK_STREAM,0);
    if(sock_fd < 0)
    {
        perror("socket");
    }
    //设置套接字让重新绑定端口
    optval = 1;
    if(setsockopt(sock_fd,SOL_SOCKET,SO_REUSEADDR,(void *)&optval,sizeof(int)) < 0)
    {
        perror("socket");
    }
    //初始化服务器端地址结构
    memset(&serv_addr,0,sizeof(struct sockaddr_in));
    serv_addr.sin_family = AF_INET;//ipv4套接字
    serv_addr.sin_port = htons (SERV_PORT);//服务器端口
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);//表示任意地址，不确定的地址

    //将套接字绑定到本地端口
    if(bind(sock_fd,(struct sockaddr *)&serv_addr,sizeof(struct sockaddr_in) ) < 0)
    {
        perror("bind");
    }
    if(listen(sock_fd,LISTENQ) < 0)
    {
        perror("listen");
    }
    cli_len = sizeof(struct sockaddr_in);
    while(1)
    {
        //通过accept接受客户端的连接请求,并返回连接套接字用于收发数据
        conn_fd = accept(sock_fd,(struct sockaddr *)&cli_addr,&cli_len);
        if(conn_fd < 0)
        {
            perror("accept");
        }
        printf("accpet a new client,ip:%s\n",inet_ntoa(cli_addr.sin_addr));//再地址中获取ip
        //创建一个子进程处理刚刚接受的连接请求
        if((pid = fork()) == 0)//子进程
        {
            while(1)
            {
                if((ret = recv(conn_fd,recv_buf,sizeof(recv_buf),0)) < 0)
                {
                    perror("recv");
                    exit(1);
                }
                recv_buf[ret-1] = '\0';//将数据结束标志'\n'替换成字符串结束标志

                if(flag_recv == USERNAME)   //接受的是用户名
                {
                    name_num = find_name(recv_buf);
                    switch(name_num)
                    {
                        case -1:
                              send_data(conn_fd,"n\n");
                              break;
                        case -2:
                              exit(1);
                              break;
                        default:
                              send_data(conn_fd,"y\n");
                              flag_recv = PASSWORD;
                              break;
                    }
                }
                else if(flag_recv == PASSWORD)
                {
                    if(strcmp(users[name_num].password,recv_buf)  == 0)
                    {
                        send_data(conn_fd,"y\n");
                        send_data(conn_fd,"Welcome login my tcp server\n");
                        printf("%s login\n",users[name_num].username);
                        break;
                    }
                    else
                    {
                        send_data(conn_fd,"n\n");
                    }
                }
            }
           close(sock_fd);
           close(conn_fd);
           exit(0);//结束子进程
        }
        else
        {//父进程关闭刚刚接受的连接请求,执行accept等待其他连接的请求
            close(conn_fd);
        }
    }
    return 0;
}












