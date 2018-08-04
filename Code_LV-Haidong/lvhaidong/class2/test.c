#include<stdio.h>
#include<unistd.h>
#include<wait.h>
int main()
{
    printf("Hello,my pid is %d\n",getpid());

    pid_t pid =fork();
    if(pid == 0)
    {
        printf("I was forked : D %d\n",getpid());
    }
    else
    {
        waitpid(pid,NULL,0);
        printf("%d was forked \n",pid);
    }
    return 0;
}
