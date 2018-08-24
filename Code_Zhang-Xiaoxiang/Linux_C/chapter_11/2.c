// connect
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>          // inet_aton 函数原型

// int connect(int sockfd, const struct sockaddr * serv_addr, socklen_t addrlen);
// sockfd : 函数socket创建的套接字
// 如果套接字类型为 SOCK_STREAM， 函数用于向服务器发出连接请求，服务器的ip地址和端口号由参数serv_addr指定
// 如果套接字类型为 SOCK_DGARM ， 函数不会创建真正的连接，只告诉内核与该套接字进行通信的目的地址，只有该目的地址发来的请求会被该socket接受
//                             好处：不必在每次发送和接受数据时都指定目的地址
// 对面向连接的套接字只能调用一次 connect 函数，对于无连接的套接字，可以多次调用函数来改变与目的地址的绑定
// 把参数　serv_addr.sa_family 设置为 AF_UNSPEC 可以取消绑定
// 
// 执行成功返回0, 错误发生返回-1，错误代码存入 errno 

int main(void)
{
    int sock_fd;
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0)
    {
        perror("socket");
        exit(1);
    }
    struct sockaddr_in  serv_addr;
    memset(&serv_addr, 0, sizeof(struct sockaddr_in));               // 把各个字段清零
    serv_addr.sin_family = AF_INET;                                  // TCP/IP　类型
    serv_addr.sin_port   = htons(80);                                // 设置端口为 80
    // inet_aton 函数将一个字符串转换成一个网络地址，并把该网络地址赋值给第二个参数
    // 如果这个函数成功，函数的返回值非零。如果输入地址不正确则会返回零。使用这个函数并没有错误码存放在errno中，所以他的值会被忽略。
    if (inet_aton("172.17.242.131", &serv_addr.sin_addr) == 0)
    {
        perror("inet_aton");
        exit(1);
    }
    // 使用sock_fd套接字连接到由serv_addr指定的目的地址上，假定sock_fd已定义
    if (connect(sock_fd, (struct sockaddr*)&serv_addr, sizeof(struct sockaddr_in)) < 0)     // ?????? 停住了
    {
        perror("connect");
        exit(1);
    }
    else
        printf("success!\n");

    return 0;
}
