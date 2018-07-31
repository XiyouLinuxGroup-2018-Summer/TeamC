#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
int main(int argc,char **argv)
{
    int i;
    printf("I am a process inmage!\n");
    printf("my pid=%d,parentpid=%d\n",getpid(),getppid());
    printf("uid= %d ,gid = %d\n",getuid(),getgid());
     for(i=0;i<argc;i++)
         printf("argv[%d]:%s\n",i,argv[i]);
}

