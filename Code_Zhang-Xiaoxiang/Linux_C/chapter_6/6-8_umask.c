/**
* 6-8_umask.c
* @author: PhoenixXC
* @email: xuancbm@xiyoulinux.org
* @description: umask 的使用
* @created: Tue Jul 24 2018 21:17:22 GMT+0800 (CST)
* @last-modified: Tue Jul 24 2018 21:17:22 GMT+0800 (CST)
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

int main(void)
{
    umask(0);       // 不屏蔽任何权限
    if (creat("example_681.test", S_IRWXU | S_IRWXG | S_IRWXO) < 0)
    {
        perror("creat");
        exit(1);
    }
    
    umask(S_IRWXO); // 屏蔽其他用户的所有权限
    if (creat("example_682.test", S_IRWXU | S_IRWXG | S_IRWXO) < 0)
    {
        perror("creat");
        exit(1);
    }
    // -rwxrwxrwx 1 xuanc xuanc 0 7月  24 21:23 example_681.test
    // -rwxrwx--- 1 xuanc xuanc 0 7月  24 21:23 example_682.test
    return 0;    
}