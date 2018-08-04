/**
* 7-10_studyuid.c
* > author: PhoenixXC
* > email: xuancbm@xiyoulinux.org
* > description: setuid setgid
* > created: Tue Jul 31 2018 15:13:08 GMT+0800 (CST)
* > last-modified: Tue Jul 31 2018 15:13:08 GMT+0800 (CST)
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

extern int errno;

int main(void)
{
    int fd;

    printf("Uid study:  \n");
    printf("Process's uid = %d, euid = %d\n", getuid(), geteuid());

    // strerror 函数获取指定错误码的提示信息
    if ((fd = open("test.c", O_RDWR)) == -1)
    {
        printf("Open failure, errno is %d: %s\n", errno, strerror(errno));
        exit(1);
    }
    else 
    {
        printf("Open successfully!\n");
    }

    close(fd);
    exit(0);
}
