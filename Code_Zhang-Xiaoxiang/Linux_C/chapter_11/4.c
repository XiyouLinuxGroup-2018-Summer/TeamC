// listen..
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/socket.h>
#include <netinet/in.h>

/**
 * 由函数socket创建的套接字是主动套接字，用来主动请求连接到某个服务器（connect）
 * 作为服务器端的程序，通常在某个端口上监听等待客户端的连接请求
 * 服务器端，先调用函数socket创建一个主动套接字，然后调用函数bind将该套接字绑定到某个端口，再调用函数listen将套接字转化为监听套接字，等待来自客户端的连接请求
 * 但listen并不能接受连接请求，只是设置为等待连接
 * 
 * 多个客户端连接到一个服务器，服务器向这些客户端提供某种服务
 * 服务器端口设置一个连接队列，记录已经建立的连接，参数backlog指定了该连接队列的最大长度，如果连接队列达到最大，之后的连接请求将被服务器拒绝
 * 
 * 执行成功返回0，有错误发生返回-1，错误代码存入errno中
 **/

#define LISTEN_NUM 12   
// 定义连接请求队列长度
// ...

int main(void)
{
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0)
    {
        perror("socket");
        exit(1);
    }
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(struct sockaddr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(80);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sock_fd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr_in)) < 0)
    {
        perror("bind");
        exit(1);
    }

    // ... 

    if (listen(sock_fd, LISTEN_NUM) < 0)
    {
        perror("listen");
        exit(1);
    }
    // ...
}

