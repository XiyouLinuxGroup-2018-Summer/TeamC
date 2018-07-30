#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
int main()
{
    pid_t pid;
    char * msg;
    int k;
    pid= fork();
    switch(pid)
    {
    case 0: msg="111111";
            k=3;
            break;
    case -1 : perror("error");
             break;
    default: msg = "running";
             k=5;
             break;
    }
    while(k>0)
    {
         puts(msg);
         sleep(1);
         k--;
    }
}
