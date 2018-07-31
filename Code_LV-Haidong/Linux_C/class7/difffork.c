#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<unistd.h>
int globVar=5;
int main(void)
{
    pid_t pid;
    int var=1,i;
    printf("fork is different vfork\n");

//    pid =fork();
    pid= vfork();
    switch(pid)
    {
        case 0: 
            i=3;
            while(i --> 0)
            {
                    printf("child process is running \n");
                    globVar++;
                    var++;
                    sleep(1);
            }
            printf("child %d , var = %d ",globVar,var);
            break;
        case -1: printf("error");
                 exit(0);

        default:
                 i=5;
                 while( i --> 0 )
                 {
                     printf("parents process is running\n");
                     globVar++;
                     var++;
                     sleep(1);
                 }
                 printf("globVar %d var = %d\n",globVar,var);
                 exit(0);
    }
}
