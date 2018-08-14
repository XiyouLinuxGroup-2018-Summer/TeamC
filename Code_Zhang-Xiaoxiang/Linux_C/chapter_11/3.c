// bind 绑定套接字
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/socket.h>
#include <netinet/in.h>

// int bind(int sockfd, struct sockaddr * my_addr, socklen_t addrlen)
// socket 只创建了一个套接字，程序没有指定套接字工作在哪个端口上
// 在客户机/服务器模型中，服务器的ip地址和端口号一般固定，在服务器端的程序中，使用bind函数将一个套接字和某个端口绑定在一起，函数只有服务器端的程序调用
// 参数my_addr指定了sockfd将绑定到的本地地址，将参数my_addr设置为INADDR_ANY可以绑定到任何网络接口
//     只有一个ip地址的计算机，INADDRO_ANY对应的就是它的ip地址
//     对于多宿主主机（有多个网卡），INADDR_ANY表示本服务器程序将处理来自所有网络接口上相应端口的连接请求
// 执行成功返回0，发生错误返回-1，错误代码存入errno中

int main(void)
{
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0)
    {
        perror("socket");
        exit(1);
    }
    struct sockaddr_in serv_addr;       // 套接字地址
    memset(&serv_addr, 0, sizeof(struct sockaddr_in));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port   = htons(80);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);      // 第三种方式了

    if (bind(sock_fd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr_in)) < 0)
    {
        // bind: Permission denied 需要权限
        perror("bind");
        exit(1);
    }
    else
        printf("Success.\n");
    
    return 0;
}
