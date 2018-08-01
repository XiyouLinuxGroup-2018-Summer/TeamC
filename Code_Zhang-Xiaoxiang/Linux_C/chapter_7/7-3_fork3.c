/**
* 7-3_fork3.c
* > author: PhoenixXC
* > email: xuancbm@xiyoulinux.org
* > description: 
* > created: Mon Jul 30 2018 14:29:05 GMT+0800 (CST)
* > last-modified: Mon Jul 30 2018 14:29:05 GMT+0800 (CST)
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main(void)
{
    pid_t pid;
    int count = 0;

    pid = fork();
    switch(pid)
    {
        case 0:
            while(count < 5)
            {
                printf("\nA background process, PID: %d, ParentID: %d\n", getpid(), getppid());
                sleep(1);
                count++;
            }
            exit(0);
            break;
        case -1:
            perror("Process creation failed.");
            exit(-1);
            break;
        default:
            printf("I am parent process, my pid is %d\n", getpid());
            exit(0);
            break;
    }

    // return 0;
    exit(0);
}