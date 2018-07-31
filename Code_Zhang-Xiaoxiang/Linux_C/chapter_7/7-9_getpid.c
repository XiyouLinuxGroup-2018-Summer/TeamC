/**
* 7-9_getpid.c
* > author: PhoenixXC
* > email: xuancbm@xiyoulinux.org
* > description: use getid
* > created: Tue Jul 31 2018 15:01:41 GMT+0800 (CST)
* > last-modified: Tue Jul 31 2018 15:01:41 GMT+0800 (CST)
*/

#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

int main(void)
{
    pid_t pid;

    if ((pid = fork()) == -1)
    {
        printf("fork error!\n");
        exit(1);
    }

    if (pid == 0)
        printf("getpid return %d\n", getpid());

    exit(0);
}
