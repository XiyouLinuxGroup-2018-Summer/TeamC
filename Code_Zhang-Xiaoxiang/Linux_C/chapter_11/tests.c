// client to client 
// ---- server ----
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <errno.h>
#include "my_recv.h"

#define SERV_PORT 80        // 服务器端口
#define LISTENQ   2         // 请求连接队列的长度

int main(void)
{
    int sock_fd, conn_fd;
    int ret, flag = 1, op;
    int epfd, nfds;
    struct epoll_event ev, events[2];

    char recvbuf[1024], sendbuf[1024]; 
    struct sockaddr_in ser_addr, cli_addr;

    // 创建一个套接字
    sock_fd = sock(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0)
        my_err("sock", __LINE__);
    // 设置套接字使之可以重新绑定端口
    op = 1;
    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEPORT, (void*)op, sizeof(int)) < 0)
        my_err("setsockopt", __LINE__);
    // 初始化服务器端地址结构
    memset(&ser_addr, 0, sizeof(struct sockaddr_in));
    ser_addr.sin_port = htons(SERV_PORT);
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    // 将套接字绑定在本地端口
    ret = bind(sock_fd, (struct sockaddr *)&ser_addr, sizeof(struct sockaddr_in));
    if (ret < 0)
        my_err("bind", __LINE__);
    // 将套接字转化为监听套接字
    ret = listen(sock_fd, LISTENQ);
    if (ret < 0)
        my_err("listen", __LINE__);
    
    int i = 0;
    while (1)
    {
        
    }
}
