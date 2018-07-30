#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
int main()
{
    pid_t pid;
    pid=fork();
    switch(pid)
    {
    case 0:   printf("%d %d \n",pid, getgid());
                   break;
        case -1: perror("error");
                 break;
        default:  printf("%d %d \n",pid,getgid());
                   break;
    }
    return 0;
        
}
