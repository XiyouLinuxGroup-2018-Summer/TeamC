/**
* 7-7_execve.c
* > author: PhoenixXC
* > email: xuancbm@xiyoulinux.org
* > description: exec 函数实例
* > created: Tue Jul 31 2018 10:12:37 GMT+0800 (CST)
* > last-modified: Tue Jul 31 2018 10:12:37 GMT+0800 (CST)
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char * argv[], char ** environ)
{
    pid_t pid;
    int stat_val;
    printf("exec example:\n");
    pid = fork();
    switch(pid)
    {
        case -1:
            perror("Process creation failed!\n");
            exit(1);
        case 0:
            printf("Child process is running\n");
            printf("My pid = %d, parent pid = %d\n", getpid(), getppid());
            printf("My uid = %d, gid = %d\n", getuid(), getgid());
            execve("ls", argv, environ);
            // if(execvp("lsk", argv) == -1)
            //     perror("");
            printf("Process never go to here!\n");
            // 映像已被代替，永远不会被执行
            exit(0);
        default:
            printf("Parent process is running\n");
            break;
    }

    int exitpid = wait(&stat_val);
    printf("child process id %d\n", exitpid);
    防止子进程成为僵死进程
    exit(0);
}
