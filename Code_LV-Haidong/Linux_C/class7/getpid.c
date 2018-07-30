//用于获得进程id
#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
#include<stdlib.h>
int main(void)
{
    pid_t pid;
    if((pid=fork()) == -1)//成功调用返回0和子进程pid
    {
        printf("fork error!\n");
        exit(1);
    }
    if(pid == 0)
    {
         printf("getpid return %d \n",getpid());
    }
}
