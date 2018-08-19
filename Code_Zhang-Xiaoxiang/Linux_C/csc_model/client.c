#include <stdio.h>
#include <time.h>
#include <signal.h>
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

// 函数：错误处理
void my_err(const char * err_string, int line)
{
    fprintf(stderr, "line: %d ", line);
    perror(err_string);
    exit(1);
}


void* thread(void * sockfd)
{
    int len;
    int sock_fd = *(int*)sockfd;
    char recvbuf[1024];
    while (1)
    {
        len = recv(sock_fd, recvbuf, 1023, 0);
        if (len < 0)
            my_err("recv", __LINE__);
        printf("%s\n", recvbuf);
    }
}

int main(void)
{
    ////////////////////////////
    signal(SIGPIPE, SIG_IGN);
    ////////////////////////////
    int ret;
    int sock_fd;                                            // 套接字描述符
    char tempname[32];                                      // 存储临时用户名
    pthread_t thd0;                                         // 存储线程id
    char recvbuf[1024];                                     // 接受缓冲区
    char tempstring[1024];                                  // 输入缓冲区

    struct sockaddr_in cli_addr;                            // 新建套接字地址结构
    memset(&cli_addr, 0, sizeof(struct sockaddr_in));       // 清空
    cli_addr.sin_family = AF_INET;                          // 设置为tcp/ip     
    cli_addr.sin_port = htons(123);                          // 设置端口
    cli_addr.sin_addr.s_addr = inet_addr("127.0.0.1");      // 设置ip

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);                // 创建套接字
    if (sock_fd < 0)
        my_err("sock", __LINE__);
    
    ret = connect(sock_fd, (struct sockaddr*) &cli_addr, sizeof(struct sockaddr_in));
    if (ret < 0)
        my_err("connect", __LINE__);
                      
    printf("输入临时用户名：\n");
    gets(tempname);
    tempname[31] = '\0';

    // send [username] - message
    ret = send(sock_fd, tempname, strlen(tempname), 0);
    if (ret < 0)
        my_err("send", __LINE__);
    
    // recv [welcome message]
    ret = recv(sock_fd, recvbuf, 1023, 0);
    if (ret < 0)
        my_err("recv", __LINE__);
    printf("Server: %s\n", recvbuf);

    // 发送接收信息
    // recv message
    ret = pthread_create(&thd0, NULL, (void*)thread, (void*)&sock_fd);
    if (ret < 0)
        my_err("pthread_create", __LINE__);
    // send message
    while (1)
    {
        gets(tempstring);
        if (tempstring[0] == '\n')
            continue;
        printf("\n");
        ret = send(sock_fd, tempstring, strlen(tempstring), 0);
        if (ret < 0)
            my_err("send", __LINE__);
        sleep(1);
    }

    pthread_join(thd0, NULL);
    close(sock_fd);
    
    return 0;
}
