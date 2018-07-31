#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
#include <stdlib.h>
int main()
{
    pid_t pid;
    int count=0;
    pid=fork();
    printf("%d\n",pid);
    switch(pid){
    case 0:{
               while(count<5)
               {
                   printf("Son :%d , Parents: %d \n",getpid(),getppid());//getpid 说明是子进程的pid ,getppid说明是父进程的pid   
                   sleep(1);
                   count++;
               }
           }
    case -1: printf("error");
             break;

    default:
             printf("I am parent process , my pid is %d\n",getpid());
             exit(0);
    }
    return 0;
}
