/**
* 8-4_tsd.c
* > author: PhoenixXC
* > email: xuancbm@xiyoulinux.org
* > description: lsd
* > created: Mon Aug 06 2018 14:05:42 GMT+0800 (CST)
* > last-modified: Mon Aug 06 2018 14:05:42 GMT+0800 (CST)
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

pthread_key_t  key;

void * thread2(void *arg)
{
    int tsd = 5;
    printf("thread %lu is running\n", pthread_self());
    pthread_setspecific(key, (void*)tsd);
    printf("thread %lu returns %d\n", pthread_self(), pthread_getspecific(key));
}

void * thread1(void *arg)
{
    int tsd = 0;
    pthread_t thid2;

    printf("thread %lu is running\n", pthread_self());
    pthread_setspecific(key, (void*) tsd);
    pthread_create(&thid2, NULL, thread2, NULL);    // 线程内部在创建一个线程
    sleep(2);
    printf("thread %lu returns %d\n", pthread_self(), pthread_getspecific(key));
    // %d 太小，会发生溢出
}

int main(void)
{
    pthread_t thid1;
    printf("main thread begins running\n");
    pthread_key_create(&key, NULL);                 // 创建
    pthread_create(&thid1, NULL, thread1, NULL);    // 创建新的线程
    sleep(3);
    pthread_key_delete(key);
    printf("main thread exit\n");
    return 0;
}
