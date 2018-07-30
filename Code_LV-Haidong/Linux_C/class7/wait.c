#include<stdio.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<stdlib.h>
#include<unistd.h>
#include<stdlib.h>
int main()
{
    pid_t pid;
    char *m;
    int k;
    int exit_code;
    printf("Study how to get exit code \n");
    pid=fork();
    switch(pid)
    {
    case 0: m="child process is running";
            k=5;
            exit_code =36;
            break;
    case -1:
            perror("process creation failed\n");
            exit(1);
    default: 
            exit_code=0;
            break;
    
    }
    //父子进程都会读取，不过父进程被阻塞等待子进程完成
    if(pid!=0)
    {
        int stat_val;
        pid_t child_pid;
        child_pid = wait(&stat_val);

        printf("exited,pid = %d",child_pid);
        if(WIFEXITED(stat_val))//一种宏，表示判断子进程是不是自己正常结束的
        {
            printf("%d\n",WEXITSTATUS(stat_val));
        } 
        else
            printf("Child exitd abnormally\n");//非正常退出
    }
    else
    {
        while(k -- > 0)
        {
              puts(m);
              sleep(1);
        }
    }
}
