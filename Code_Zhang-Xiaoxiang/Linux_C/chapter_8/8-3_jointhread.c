/**
* 8-3_jointhread.c
* > author: PhoenixXC
* > email: xuancbm@xiyoulinux.org
* > description: jointhread
* > created: Mon Aug 06 2018 10:59:20 GMT+0800 (CST)
* > last-modified: Mon Aug 06 2018 10:59:20 GMT+0800 (CST)
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void assisthread(void * arg)
{
    printf("I am helping to do some jobs\n");
    sleep(3);
    pthread_exit(0);
}

int main(void)
{
    pthread_t assistthid;  
    int       status;

    pthread_create(&assistthid, NULL, (void*) assisthread, NULL);
    pthread_join(assistthid, (void* )&status);
    printf("assisthread's exit is caused %d\n", status);

    return 0;
}
