/**
* 7-5_daemon.c
* > author: PhoenixXC
* > email: xuancbm@xiyoulinux.org
* > description: 守护进程
* > created: Mon Jul 30 2018 16:35:26 GMT+0800 (CST)
* > last-modified: Mon Jul 30 2018 16:35:26 GMT+0800 (CST)
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <syslog.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

int init_daemon(void)
{
    int pid;
    int i;

    // 忽略终端 I/O信号， STOP 信号
    signal(SIGTTOU, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    signal(SIGHUP , SIG_IGN);

    pid = fork();
    if (pid > 0)
    {
        exit(0);    // 结束父进程
    }
    else if (pid < 0)
    {
        return -1;
    }

    // 建立一个新的进程组，在这个新的进程组中，子进程成为这个进程组的首进程，以使该进程脱离所有终端
    setsid();

    // 再次建立一个子进程，退出父进程，保证该进程不是进程组长，同时让该进程无法打开一个新的终端
    pid = fork();
    if (pid > 0)
    {
        exit(0);
    }
    else if (pid < 0)
    {
        return -1;
    }

    // 关闭所有从父进程继承的不需要的文件描述符
    for (i = 0; i < NOFILE; close(i++));

    // 改变工作目录
    chdir("/");

    // 文件屏蔽字设为0
    umask(0);

    // 忽略 SIGGHLD 信号
    signal(SIGCHLD, SIG_IGN);

    return 0;
}

int main()
{
    time_t now;
    init_daemon();
    syslog(LOG_USER | LOG_INFO, "测试守护进程！\n");
    while(1)
    {
        sleep(8);
        time(&now);
        syslog(LOG_USER | LOG_INFO, "系统时间：\t%s\t\t\n", ctime(&now));
    }
}