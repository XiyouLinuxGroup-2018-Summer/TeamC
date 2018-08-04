#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
#include<sys/resource.h>
#include<sys/wait.h>
#include<stdlib.h>
int main()
{
    pid_t pid;
    int stat_val=0;
    int oldprl,newprl;
    pid=fork();//创建子进程
    switch(pid)
    {
        case 0: printf("Child is runnning , pid is %d , Parentpid is %d \n",pid,getppid());
                oldprl = getpriority(PRIO_PROCESS,0);//获取特定进程的优先级
                printf("Old priority = %d\n",oldprl);
                newprl = nice(2);
                printf("New priority = %d \n",newprl);
                exit(0);
        case 1:  perror("Process creation failed \n");
                 break;
         default: printf("Parent is runnning , Childpid is %d ,Parentpid is %d\n",pid,getpid());
                 break;
    }
    wait(&stat_val);
    exit(0);
}
