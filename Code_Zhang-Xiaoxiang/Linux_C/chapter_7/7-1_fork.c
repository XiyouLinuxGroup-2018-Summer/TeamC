/**
* 7-1_fork.c
* @author: PhoenixXC
* @email: xuancbm@xiyoulinux.org
* @description: 
* @created: Mon Jul 30 2018 11:19:44 GMT+0800 (CST)
* @last-modified: Mon Jul 30 2018 11:19:44 GMT+0800 (CST)
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main(void)
{
    pid_t pid;

    printf("Process Creation Study\n");
    pid = fork();
    switch(pid)
    {
        case 0:
            printf("Child process is running, CurPid is %d, ParentPid is %d\n", pid, getppid());
            break;
        case -1: 
            printf("Process creation failed\n");
            break;
        default:
            printf("Parent process is running, ChildPid is %d, ParentPid is %d\n", pid, getpid());
            break;
    }

    exit(0);
}
