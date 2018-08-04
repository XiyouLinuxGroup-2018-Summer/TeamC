#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
int main(void)
{
    int pid;
    pid = fork();
    char * arg[3] = {
        "ls"
        "-l -a",
        NULL
    };
    char * arg1[3]= {
        "gcc",
        "-v",
        NULL
    };
    if (pid == 0)
    {
        for (int i = 0; i < 2; i++)
        {
            if (i == 0)
                execvp(arg[0], arg);
            else 
                execvp(arg1[0], arg1);
        }
        exit(0);
    }
    return 0;
}
