#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
#include<stdlib.h>
#include <sys/wait.h>
int main(int argc,char ** argv,char ** environ)
{ 
    //environ 显示全部变量的值
  pid_t pid;
  int stat_val;
  printf("EXec example\n");
  pid=fork();
  switch(pid)
  {
   case -1:
           perror("perror");
           exit(1);
    case 0:
          printf("Child process is running\n");
          printf("my pid =%d,parentpid= %d\n",getpid(),getppid());
          printf("uid = %d , gid = %d\n",getuid(),getgid());
          execve("processimage",argv,environ);
          printf("process never go to here!\n");
          exit(0);
    default: 
          printf("parent process is running\n");
         break;
  }
  wait(&stat_val);
    return 0;
}
