#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
int main()
{
    pid_t pid;
    pid = fork();
    if (pid < 0)
    {
        perror("fork error:");
        exit(1);
    }
    else if (pid == 0)
    {
        printf("I am child process.I amexiting.\n");
        exit(0);
    }
    /* 父进程部分*/
    printf("I am father process.I willsleep two seconds\n");
    //等待子进程先退出
    sleep(3);
    //输出进程信息
    system("ps -opid,ppid,state,tty,command");
    printf("father process isexiting.\n");
    return 0;
}
