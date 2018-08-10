/**
* 8-1_createthread.c
* > author: PhoenixXC
* > email: xuancbm@xiyoulinux.org
* > description: 
* > created: Mon Aug 06 2018 09:43:53 GMT+0800 (CST)
* > last-modified: Mon Aug 06 2018 09:43:53 GMT+0800 (CST)
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

int * thread(void * arg)
{
    pthread_t newthid;

    newthid = pthread_self();                                    // 返回新创建线程的ID
    printf("this is a new thread, thread ID = %u\n", newthid);
    return NULL;
}

int main(int argc, char const *argv[])
{
    pthread_t thid;

    printf("main thread, ID is %u\n", pthread_self());          // 打印主线程ID
    if (pthread_create(&thid, NULL, (void *)thread, NULL) != 0) // 创建一个线程, 创建成功返回 0
    {
        printf("thread creation failed\n");
        exit(1);
    }
    sleep(1);
    return 0;
}
