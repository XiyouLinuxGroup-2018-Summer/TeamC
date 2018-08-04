#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<errno.h>
extern int errno;
int main()
{
    int fd;
    printf("uid study:\n");
    printf("Process uid = %d , euid= %d \n",getuid(),geteuid());
    //strerror函数获取指定错误代码的提示信息
    if((fd=open("nihao",O_RDWR))==-1)
    {
        printf("OPEN failure ,erro is %d \n",errno,perror(errno));
        exit(1);
    }
    else{
        printf("open successfully\n");
    }
    close(fd);
    exit(0);   
}










