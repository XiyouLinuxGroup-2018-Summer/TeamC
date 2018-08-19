// #define DEBUG
#include <stdio.h>
#include <signal.h>
#include <time.h>
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

int sockfds[50] = {0};
pthread_t thds[50] = {0};
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

#define SERV_PORT 123         // 服务器端口
#define LISTENQ   50         // 请求连接队列的长度

// 函数：错误处理
void my_err(const char * err_string, int line)
{
    fprintf(stderr, "line: %d ", line);
    perror(err_string);
    exit(1);
}

// 获取当前时间
void get_time(char * string)
{
    time_t now;                 //实例化time_t结构
    struct tm * timenow;        //实例化tm结构指针
    time(&now);
    //time函数读取现在的时间(国际标准时间非北京时间)，然后传值给now
    timenow = localtime(&now);
    //localtime函数把从time取得的时间now换算成你电脑中的时间(就是你设置的地区)
    // printf("Local time is %s\n",asctime(timenow));
    strcpy(string, asctime(timenow));
}

void* thread(void * sockfd)
{
    int len, flag = 1;
    int sock_fd = *(int*)sockfd;
    char name[32], recvbuf[1024], sendbuf[1024], sendtime[32];
    len = recv(sock_fd, name, 32, 0);
    if (len < 0)
    {
        fprintf(stderr, "%d: ", __LINE__);
        perror("recv");
        pthread_exit(0);
    }
    len = send(sock_fd, "welcome to join group\n", 22, 0);
    if (len < 0)
    {
        fprintf(stderr, "%d: ", __LINE__);
        perror("send");
        pthread_exit(0);
    }

    while(1)
    {
        memset(sendbuf, '\0', sizeof(sendbuf));
        memset(recvbuf, '\0', sizeof(recvbuf));
        len = recv(sock_fd, recvbuf, 1024, 0);
        if (len < 0)
        {
            fprintf(stderr, "%d: ", __LINE__);
            perror("recv");
            pthread_exit(0);
        }
        if (len == 0)
        {
            sprintf(sendbuf, "[System Message] %s quit group\n", name);
            flag = 0;
        }
        get_time(sendtime);
        if (flag)
        {
            sprintf(sendbuf, "%s[%s] \"%s\"", sendtime, name, recvbuf);
        }
        for (int i = 0; i < 51; i++)
        {
            if (sockfds[i] == 0)
                continue;
            if (sockfds[i] == sock_fd)
            {
                pthread_mutex_lock(&mutex);
                if (flag == 0)
                    sockfds[i] = 0;
                pthread_mutex_unlock(&mutex);
                continue;
            }
            pthread_mutex_lock(&mutex);
            len = send(sockfds[i], sendbuf, 1024, 0);
            pthread_mutex_unlock(&mutex);
            if (len < 0)
            {
                fprintf(stderr, "%d: ", __LINE__);
                perror("send");
                pthread_exit(0);
            }
        }
        if (flag == 0)
            pthread_exit(0);
    }
    pthread_exit(0);    
}

int main(void)
{
    ////////////////////////////
    signal(SIGPIPE, SIG_IGN);
    ////////////////////////////
    int sock_fd, conn_fd;
    int ret, op, pot;
    int epfd, nfds;
    struct epoll_event ev, events[2];

    char recvbuf[1024], sendbuf[1024]; 
    struct sockaddr_in ser_addr, cli_addr;

    // 创建一个套接字
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0)
        my_err("sock", __LINE__);
    // 设置套接字使之可以重新绑定端口
    op = 1;
    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEPORT, (void*)&op, sizeof(int)) < 0)
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
    
    pot = 0;
    while (pot < 51)
    {
        int cli_len = sizeof(struct sockaddr_in);
        sockfds[pot] = accept(sock_fd, (struct sockaddr *)&cli_addr, &cli_len);
        if (sockfds[pot] < 0)
            my_err("sockfds", __LINE__);
        ret = pthread_create(&(thds[pot]), NULL, (void*)thread, (void*)&(sockfds[pot]));
        if (ret < 0)
            my_err("pthread_create", __LINE__);
        pot++;
        printf("目前有 %d 个连接\n", pot);
    }
    
    for (int i = 0; i < pot; i++)
    {
        close(sockfds[i]);
        pthread_join(thds[i], NULL);
    }
    pthread_mutex_destroy(&mutex);
    return 0;
}
