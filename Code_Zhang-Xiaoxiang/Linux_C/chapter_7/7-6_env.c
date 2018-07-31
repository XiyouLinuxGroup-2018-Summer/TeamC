/**
* 7-6_env.c
* > author: PhoenixXC
* > email: xuancbm@xiyoulinux.org
* > description: 
* > created: Mon Jul 30 2018 21:54:42 GMT+0800 (CST)
* > last-modified: Mon Jul 30 2018 21:54:42 GMT+0800 (CST)
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <malloc.h>

extern char ** environ;
// 预定义的变量、显示各个环境变量的值

int main(int argc, char * argv[], char **envp)
{
    int i;

    printf("Arguement:\n");
    for (i = 0; i < argc; i++)
        printf("argv[%d] is %s\n", i, argv[i]);

    // printf("Enviroment:\n");

    // for (i = 0; environ[i] != NULL; i++)
    //     printf("%s\n", environ[i]);
    // environ — array of character pointers to the environment strings

    printf("\nEnvp from the main function third Arguement:\n");
    for (i = 0; envp[i] != NULL; i++)
        printf("%s\n", envp[i]);

    return 0;
}