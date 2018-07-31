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

int main(int argc, char * argv[])
{
    int i;

    printf("Arguement:\n");
    for (i = 0; i < argc; i++)
        printf("argv[%d] is %s\n", i, argv[i]);

    printf("Enviroment:\n");

    for (i = 0; environ[i] != NULL; i++)
        printf("%s\n", environ[i]);

    return 0;
}