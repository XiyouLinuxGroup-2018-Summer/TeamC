#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<errno.h>
int main()
{
    int fd;
    if((fd=open("nihao.txt",O_CREAT|O_EXCL,S_IRUSR|S_IWUSR))==-1){
               perror("open");
               exit(1);
       }
    else{
        printf("替换成功");
    }
   close(fd);
   
   
    return 0;
}
