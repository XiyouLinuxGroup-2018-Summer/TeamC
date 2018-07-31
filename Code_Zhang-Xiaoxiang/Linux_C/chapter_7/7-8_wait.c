/**
* 7-8_wait.c
* > author: PhoenixXC
* > email: xuancbm@xiyoulinux.org
* > description: use wait()
* > created: Tue Jul 31 2018 10:52:22 GMT+0800 (CST)
* > last-modified: Tue Jul 31 2018 10:52:22 GMT+0800 (CST)
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

int main()
{
    pid_t pid;
    char * msg;
    int k;
    int exit_code;

    printf("Study how to get exit code\n");
    pid = fork();
    switch(pid)
    {
        case 0:
            msg = "Child process is running.";
            // emm...
            k = 5;
            exit_code = 37;
            break;
        case -1:
            perror("Process creation failed.\n");
            exit(1);
        default:
            exit_code = 0;
            break;
    }

// 父子进程都会执行以下代码
// 子进程pid值为0， 父进程pid值为子进程的id
    if (pid != 0)   // 父进程
    {
        int stat_val;
        pid_t child_pid;
        
        child_pid = wait(&stat_val);    // 子进程已经退出返回child_pid，尚未退出返回0

        printf("Child process has exited, pid = %d\n", child_pid);

        if (WIFEXITED(stat_val))
            printf("Child exited with code %d\n", WEXITSTATUS(stat_val));
        else
            printf("Child exited abnormaily\n");
    }
    else
    {
        while (k-- > 0)
        {
            puts(msg);
            sleep(1);
        }
    }

    exit(exit_code);
}
