/**
* 7-2_fork2.c
* > author: PhoenixXC
* > email: xuancbm@xiyoulinux.org
* > description: 
* > created: Mon Jul 30 2018 14:21:13 GMT+0800 (CST)
* > last-modified: Mon Jul 30 2018 14:21:13 GMT+0800 (CST)
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main(void)
{
    pid_t  pid;
    char * msg;
    int    k;

    printf("Process Creation Study\n");
    pid = fork();
    switch(pid)
    {
        case 0:
            msg = "Child process is running";
            k = 3;
            break;
        case -1:
            perror("Process creation failed.\n");
            break;
        default:
            msg = "Parent process is running";
            k = 5;
            break;
    }

    while(k > 0)
    {
        puts(msg);
        sleep(1);
        k--;
    }

    exit(0);
}