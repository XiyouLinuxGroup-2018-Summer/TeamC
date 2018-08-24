// client to client 
// ---- client ----

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

int flag = 1;

void thread0(void * sockfd)                                    // for 接受消息
{
    int epfd, nfds, ret;
    epfd = epoll_create(1);
    struct epoll_event ev, events[2];
    int sock_fd = (int)sockfd;
    char recvbuf[1024];               // 接受输入缓冲区
    ev.data.fd = sock_fd;
    ev.events = EPOLLIN;
    epoll_ctl(epfd, EPOLL_CTL_ADD, sock_fd, &ev);
    while (1)
    {
        nfds = epoll_wait(epfd, events, 1, -1);
        if (nfds == 0)
        {
            fprintf(stderr, "%d: epoll_wait error\n", __LINE__);
            break;
        }
        ret = recv(sock_fd, recvbuf, 1023, 0);
        if (ret < 0)
        {
            fprintf(stderr, "%d: recv error\n", __LINE__);
            break;
        }
        if (strcmp("quit", recvbuf) == 0)
        {
            printf("对方已下线\n");
            flag = 0;
            break;
        }
        get_time();
        printf("[Message] %s\n", recvbuf);
    }
    pthread_exit(0);
}

int main(void)
{
    int ret;
    int sock_fd;                                            // 套接字描述符
    char tempname[32];                                      // 存储临时用户名
    int epfd, nfds;                                         // for epoll
    pthread_t thd0;                                         // 存储线程id
    struct epoll_event ev, events[2];                       // for epoll
    char recvbuf[1024];                                     // 接受缓冲区
    char tempstring[1024];                                  // 输入缓冲区

    struct sockaddr_in cli_addr;                            // 新建套接字地址结构
    memset(&cli_addr, 0, sizeof(struct sockaddr_in));       // 清空
    cli_addr.sin_family = AF_INET;                          // 设置为tcp/ip     
    cli_addr.sin_port = htons(80);                          // 设置端口
    cli_addr.sin_addr.s_addr = inet_addr("127.0.0.1");      // 设置ip

    sock_fd = sock(AF_INET, SOCK_STREAM, 0);                // 创建套接字
    if (sock_fd < 0)
        my_err("sock", __LINE__);
    
    ret = connect(sock_fd, (struct sockaddr*) &cli_addr, sizeof(struct sockaddr_in));
    if (ret < 0)
        my_err("connect", __LINE__);
    
    strcpc(tempstring, "[username]");                       
    printf("输入临时用户名：\n");
    gets(tempname);
    tempname[31] = '\0';
    strcpy(tempstring, tempname);

    // send [username] - message
    ret = send(sock_fd, tempstring, strlen(tempstring), 0);
    if (ret < 0)
        my_err("send", __LINE__);
    
    // wait ....
    epfd = epoll_create(1);
    ev.data.fd = sock_fd;
    ev.events = EPOLLIN;          // 监听读状态，设置为LT
    epoll_ctl(epfd, EPOLL_CTL_ADD, sock_fd, &ev);
    nfds = epoll_wait(epfd, events, 1, -1);
    if (nfds == 0)
        my_err("epoll_wait", __LINE__);
    // if (events[0].data.fd == sock_fd)
    // {
        ret = recv(sock_fd, recvbuf, 1023, 0);
        if (ret < 0)
            my_err("recv", __LINE__);
    // }
    epoll_ctl(epfd, EPOLL_CTL_DEL, sock_fd, &ev);
    // 打印从服务端接受到的信息
    printf("Server: %s\n", recvbuf);

    // 接受信息
    ret = pthread_create(&thd0, NULL, (void *)thread0, (void *)sock_fd);
    if (ret < 0)
        my_err("pthread_create", __LINE__);
    // 发送信息
    while (1)
    {
        if (flag == 0)
            break;
        gets(tempstring);
        ret = send(sock_fd, tempstring, strlen(tempstring), 0);
        if (ret < 0)
            my_err("send", __LINE__);
    }

    pthread_join(thd0, NULL);
    close(sock_fd);
    
    return 0;
}
