/**
* 7-7_processimage.c
* > author: PhoenixXC
* > email: xuancbm@xiyoulinux.org
* > description: 替换进程映像
* > created: Tue Jul 31 2018 10:07:03 GMT+0800 (CST)
* > last-modified: Tue Jul 31 2018 10:07:03 GMT+0800 (CST)
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

int main(int argc, char * argv[], char ** environ)
{
    int i;
    
    printf("I am a process image.\n");
    printf("My pid = %d, parent pid = %d\n", getpid(), getppid());
    printf("uid = %d, gid = %d\n", getuid(), getgid());

    for (i = 0; i < argc; i++)
    {
        printf("argv[%d]: %s\n", i, argv[i]);
    }
}
