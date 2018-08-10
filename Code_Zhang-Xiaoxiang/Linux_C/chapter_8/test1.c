#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

int err;
pthread_t tid;

void * thread_fun(void * arg)
{
    printf("I'm new thread\n");
    printf("I'm about to cancel myself\n");
    // 设置取消状态
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    // 取消自己
    err = pthread_cancel(pthread_self());
    puts("puts function is a cancellation points\n");
    if (err == 0)
        printf("cancel myself success\n");
    else
        printf("cancel myself failed.\n");
    pthread_exit((void*)0);
}

int main()
{
    err = pthread_create(&tid, NULL, (void *)thread_fun, NULL);
    if (err != 0)
    {
        printf("create new thread1 failed\n");
        return 0;
    }

    err = pthread_join(tid, NULL);
    if (err != 0)
        printf("main thread join new thread failed.\n");
    else
        printf("main thread join new thread success\n");

    return 0;
}   
