/**
* my_server.c
* > author: PhoenixXC
* > email: xuancbm@xiyoulinux.org
* > description: Client/Server 模型的服务器端
* > created: Mon Aug 13 2018 16:14:23 GMT+0800 (CST)
* > last-modified: Mon Aug 13 2018 16:14:23 GMT+0800 (CST)
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include "my_recv.h"

#define SERV_PORT       4507    // 服务器的端口
#define LISTENQ         12      // 连接请求队列的最大长度

#define INVALID_USERINFO 'n'    // 用户信息无效
#define VALID_USEINFO    'y'    // 用户信息有效

#define USERNAME          0     // 接收到的是用户名
#define PASSWORD          1     // 接受到的是密码

struct userinfo {               // 保存用户名和密码的结构体
    char username[32];
    char password[32];  
};
struct userinfo users[] = {
    {"linux",   "unix"},
    {"4507",    "4508"},
    {"cin",     "cin" },
    {"xl",      "xl"  },
    {" ",       " "   }         // 以只含有一个空格的字符串作为数组的结束标志
};  

// 查找用户名是否存在，存在返回该用户的下标，不存在返回-1，出错返回-2
int find_name(const char * name)
{
    int i;

    if (name == NULL)
    {
        printf("in find_name, NULL pointer");
        return -2;
    }
    for (i = 0; users[i].username[0] != ' '; i++)
    {
        if (strcmp(users[i].username, name) == 0)
            return i;
    }

    return -1;
}

// 发送数据
void send_data(int conn_fd, const char * string)
{
    if (send(conn_fd, string, strlen(string), 0) < 0)
        my_err("send", __LINE__);               // my_err 在函数my_recv.h中声明
}

int main()
{
    int         sock_fd, conn_fd;
    int         optval;
    int         flag_recv = USERNAME;           // 标志接受到的是用户名还是密码
    int         ret;
    int         name_num;
    pid_t       pid;
    socklen_t   cli_len;
    struct      sockaddr_in cli_addr, serv_addr;    // 等价于套接字地址结构
    char        recv_buf[128];

    // 创建一个TCP套接字
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);      // 创建一个TCP套接字
    if (sock_fd < 0)
        my_err("socket", __LINE__);
    
    // 设置该套接字使之可以重新绑定端口
    optval = 1;
    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, (void *)&optval, sizeof(int)) < 0)
        my_err("setsockopt", __LINE__);

    // 初始化服务器地址结构
    memset(&serv_addr, 0, sizeof(struct sockaddr_in));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // 将套接字绑定到本地端口
    if (bind(sock_fd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr_in)) < 0)
        my_err("bind", __LINE__);
    
    // 将套接字转化为监听套接字
    if (listen(sock_fd, LISTENQ) < 0)
        my_err("listen", __LINE__);

    cli_len = sizeof(struct sockaddr_in);

    while (1)
    {
        // 通过accept接受客户端的连接请求，并返回连接套接字用于收发数据
        conn_fd = accept(sock_fd, (struct sockaddr *)&cli_addr, &cli_len);
        if (conn_fd < 0)
            my_err("accept", __LINE__);
    
        printf("accept a new client, ip: %s\n", inet_ntoa(cli_addr.sin_addr));

        // 创建一个子进程处理刚刚接受的连接请求
        if ((pid = fork()) == 0)
        {
            while(1)
            {
                if((ret = recv(conn_fd, recv_buf, sizeof(recv_buf), 0)) < 0)
                {
                    perror("recv");
                    exit(1);
                }
                recv_buf[ret - 1] = '\0';         // 将数据结束标志'\n'替换成字符串结束标志

                if (flag_recv == USERNAME)
                {
                    name_num = find_name(recv_buf);
                    switch(name_num)
                    {
                        case -1:
                            send_data(conn_fd, "n\n");
                            break;
                        case -2:
                            exit(1);
                            break;
                        default: 
                            send_data(conn_fd, "y\n");
                            flag_recv = PASSWORD;
                            break;
                    }
                }
                else if (flag_recv == PASSWORD)
                {
                    if (strcmp(users[name_num].password, recv_buf) == 0)
                    {
                        send_data(conn_fd, "y\n");
                        send_data(conn_fd, "Welcome login my tcp server\n");
                        printf("%s login\n", users[name_num].username);
                        break;
                    }
                    else    
                        send_data(conn_fd, "n\n");
                }
            }
            close(sock_fd);
            close(conn_fd);
            exit(0);        // 结束子进程
        }
        else
            close(conn_fd); // 父进程关闭刚刚接受的连接请求，执行accept等待其他连接请求
    }

    return 0;
}
