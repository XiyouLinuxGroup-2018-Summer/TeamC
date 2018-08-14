// accept
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/socket.h>
#include <netinet/in.h>

// int accept(int s, struct sockaddr * addr, socklen_t * addrlen);
// s：由函数socket创建，经函数bind绑定到本地的某一端口上，然后通过函数listen转化来的套接字
// addr：　　保存发起来连接请求的主机地址和端口
// addrlen： addr所指向的结构体的大小
// 执行成功返回一个新的代表客户端的套接字，出错返回-1, 错误代码存入errno中
//
// 只能对面向连接的套接字使用accept函数，执行成功将创建一个新的套接字，并为这个套接字分配一个套接字描述符，返回这个新的套接字描述符
// 这个新的套接字描述符与打开文件时返回的文件描述符类似，进程可以利用这个新的套接字描述符与客户端进行交换数据，参数s所指定的套接字继续等待客户端的连接请求
// 如果参数s指定的套接字被设置为阻塞方式（Linux下默认方式），且请求队列为空，则accept()将被阻塞直到有连接请求到达为止，
// 若被设置为非阻塞方式，如果队列为空，accept将立即返回-1，errno被设置为EAGAIN

int main(void)
{
    int sock_fd;
    int client_fd;
    int client_len;
    struct sockaddr_in client_addr;
    // ...
    client_len = sizeof(struct sockaddr_in);
    client_fd = accept(sock_fd, (struct sockaddr*)&client_addr, &client_len);
    if (client_fd < 0)
    {
        perror("accept");
        exit(1);
    }
    // ...
}
