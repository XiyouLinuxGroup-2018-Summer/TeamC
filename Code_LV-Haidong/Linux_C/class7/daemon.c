//创建守护进程
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>
#include<sys/stat.h>
#include<sys/param.h>
#include<time.h>
#include<syslog.h>
int init_daemon(void)
{
    int pid;
    int i;
    //忽视终端I/O信号
    signal(SIGTTOU,SIG_IGN);
    signal(SIGTTIN,SIG_IGN);
    signal(SIGTSTP,SIG_IGN);
    signal(SIGTSTP,SIG_IGN);

    pid=fork();
    if(pid>0)
    {
        exit(0);//结束父进程,使得子进程成为后台进程
    }
    else if(pid<0)
    {
        return -1;
    }
    //建立一个新的进程族,在这个进程中,儿子进程成为这个进程的首进程,让该进程脱离终端
    setsid();
    //新建一个子进程,退出父亲进程,保证该进程不是进程组长,同时让该进程无法打开一个新的终端i
    //如果是进程组长的话setsid是失败的
    pid=fork();
    if(pid>0)
    {
        exit(0);
    }
    else if(pid<0)
    {
        return -1;
    }
    //关闭继承的不需要的文件描述符
    for(i=0;i<NOFILE;close(i++))//NOFILE表示的是最大的文件打开数量
   
        //改变工作目录，与文件系统不再联系
        chdir("/");

    //将文件屏蔽字设置为0,防止权限被拒绝
     umask(0);
    //忽视产生SIGHLD信号
     signal(SIGCHLD,SIG_IGN);//生成子进程的信号
     return 0;
}
int main()
{
    time_t now;
    init_daemon();
    syslog(LOG_USER | LOG_INFO ,"测试守护进程\n");
    while(1)
    {
        time(&now);
        syslog(LOG_USER | LOG_INFO,"系统时间 : \t%s\t\t\n",ctime(&now));
    }
    return 0;
}
