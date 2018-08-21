#include "base.h"
#include "internet.h"
#include "account.h"
#include <vector>
#include <set>
#include <map>
using namespace std;

#define LISTENUM            999     // 请求连接队列长度
#define EVENTS_MAX          1000    // epoll 监测的最多事件

// set<int>member;
// 群[id].member --> set
map<int, in_addr>SockWithIp;        // socket --> IP
map<int, int>IdWithSock;            // user_id --> socket

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;  // 初始化锁

void Initia(void);                  // 初始化
void EndEvents(void);               // 关闭连接，套接字等等
void* pthreadFun(void * arg);       // 线程，处理请求

int main(void)
{
    signal(SIGPIPE, SIG_IGN);       // 屏蔽pipe信号, 防止客户端异常退出后服务端挂掉
    int ret, op;
    int sock_fd, conn_fd;           // 套接字
    int epfd, nfds;
    struct epoll_event ev, events[EVENTS_MAX];
    char recvbuf[1024], sendbuf[1024]; 
    struct sockaddr_in ser_addr, cli_addr;
    pthread_t pth;                  // 线程id
    socklen_t cli_len = sizeof(struct sockaddr_in);

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0)
        my_err(__FILE__, "sock", __LINE__);
    op = 1;
    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEPORT, (void*)&op, sizeof(int)) < 0)
        my_err(__FILE__, "setsockopt", __LINE__);

    // 初始化服务器端地址结构
    memset(&ser_addr, 0, sizeof(struct sockaddr_in));
    ser_addr.sin_port = htons(PORT);
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // 将套接字绑定在本地端口
    ret = bind(sock_fd, (struct sockaddr *)&ser_addr, sizeof(struct sockaddr_in));
    if (ret < 0)
        my_err(__FILE__, "bind", __LINE__);

    // 将套接字转化为监听套接字
    ret = listen(sock_fd, LISTENUM);
    if (ret < 0)
        my_err(__FILE__, "listen", __LINE__);

    Initia();                       
    // mysql 初始化，用户、群列表，群成员信息, 用户状态 几个stl初始化, 清空

    epfd = epoll_create(EVENTS_MAX - 5);           
    ev.data.fd = sock_fd;
    ev.events = EPOLLIN;
    epoll_ctl(epfd, EPOLL_CTL_ADD, sock_fd, &ev);

    while (1)
    {
        nfds = epoll_wait(epfd, events, EVENTS_MAX, -1);

        for (int i = 0; i < nfds; i++)
        {
            if (events[i].data.fd == sock_fd)       // accept
            {
                conn_fd = accept(sock_fd, (struct sockaddr*)&cli_addr, &cli_len);
                if (conn_fd < 0)
                    my_err(__FILE__, "accept", __LINE__);
                
                SockWithIp[conn_fd] = cli_addr.sin_addr;

                get_time(NULL);
                fprintf(stdout, "[CMD] accept [IP] %s [PORT] %s", inet_ntoa(cli_addr.sin_addr), cli_addr.sin_port);

                ev.data.fd = conn_fd;
                ev.events = EPOLLIN;
                epoll_ctl
            }
            else if (events[i].events & EPOLLIN)    // 收到package, 处理
            {
                conn_fd = events[i].data.fd;
                if (pthread_create(&pth, NULL, (void*)pthreadFun, (void*)&conn_fd) < 0)
                    my_err(__FILE__, "pthread_create", __LINE__);
            }
        }
    }

    EndEvents();
    return 0;
}

