/**
* 7-4_diffork.c
* > author: PhoenixXC
* > email: xuancbm@xiyoulinux.org
* > description: using vfork()
* > created: Mon Jul 30 2018 14:49:47 GMT+0800 (CST)
* > last-modified: Mon Jul 30 2018 14:49:47 GMT+0800 (CST)
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int globVar = 5;

int main(void)
{
    pid_t pid;
    int var = 1, i;

    printf("Fork is different with vfork \n");

    // pid = fork();   // 创建了一个子进程
    pid = vfork();
    switch(pid)
    {
        case 0:
            i = 3;
            while (i-- > 0)
            {
                printf("Child process is running.\n");
                globVar++;      // 数据段
                var++;          // 堆
                sleep(1);
            }
            printf("Child's globVar = %d, var = %d\n", globVar, var);
            printf("Child's globVar at %p, var = %p\n", &globVar, &var);
            _exit(0);   
            // 结束进程
            // break;   return 会导致子进程和父进程共享的内容被释放
        case -1:
            perror("Process creation failed.");
            exit(0);
        default:
            i = 5;
            while (i-- > 0)
            {
                printf("Parent process is running\n");
                globVar++;
                var++;
                sleep(1);
            }
            printf("Parent's globVar = %d, var = %d\n", globVar, var);
            printf("Parent's globVar at %p, var = %p\n", &globVar, &var);
            exit(0);
    }
}