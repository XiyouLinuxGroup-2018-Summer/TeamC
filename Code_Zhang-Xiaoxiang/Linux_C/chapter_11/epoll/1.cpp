#include <unistd.h>
#include <iostream>
#include <sys/epoll.h>
using namespace std;
int main(void)
{
    int epfd,nfds;
    struct epoll_event ev,events[5];    //ev用于注册事件，数组用于返回要处理的事件
    epfd = epoll_create(1);               //只需要监听一个描述符——标准输出
    ev.data.fd = STDOUT_FILENO;
    ev.events = EPOLLOUT|EPOLLET;         //监听写状态同时设置ET模式
    epoll_ctl(epfd, EPOLL_CTL_ADD, STDOUT_FILENO, &ev);//注册epoll事件
    for(;;)
    {
       nfds=epoll_wait(epfd,events,5,-1);
       for(int i=0;i<nfds;i++)
       {
            if(events[i].data.fd==STDOUT_FILENO)
                cout<<"hello world!";
       }
    }
};
