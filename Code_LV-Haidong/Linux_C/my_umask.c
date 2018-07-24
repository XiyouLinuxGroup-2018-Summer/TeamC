#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
int main(void)
{
   umask(0);  ///在这里不屏蔽任何的权限
   //在这里直接给的是满权限
   //S_IRWXU分别表示文件所有者，所有族，所有人的权限
   if(creat("example_00",S_IRWXU | S_IRWXO)<0)
   {
       perror("creat");
       return 1;
   }
   umask(S_IRWXO);//屏蔽了其他用户的权限
   if(creat("example99",S_IRWXU | S_IRWXO)<0)
   {
        perror("create");
        return 1;
   }
       return 0;

}
