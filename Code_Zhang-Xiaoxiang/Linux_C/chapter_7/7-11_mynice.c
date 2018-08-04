/**
* Untitled-1
* > author: PhoenixXC
* > email: xuancbm@xiyoulinux.org
* > description: nice()
* > created: Tue Jul 31 2018 15:53:19 GMT+0800 (CST)
* > last-modified: Tue Jul 31 2018 15:53:19 GMT+0800 (CST)
*/

#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/wait.h>

int main(void)
{
    pid_t pid;
    int stat_val = 0;
    int oldpri, newpri;

    printf("nice study.\n");

    pid = fork();
    switch(pid)
    {
        case 0:
            printf("Child is running , CurPid is %d, ParentPid = %d\n", pid, getppid());
            exit(0);
        case -1:
            perror("Process creation failed!\n");
            break;
        default:
            printf("Parent is running, ChildPid is %d, ParentPid is %d\n", pid, getpid());

            // 获取子进程优先级
            oldpri = getpriority(PRIO_PROCESS, pid);
            printf("Old priority = %d\n", oldpri);

            newpri = nice(2);
            printf("New priority = %d\n", newpri);

            break;
    }

    wait(&stat_val);
    exit(0);
}
