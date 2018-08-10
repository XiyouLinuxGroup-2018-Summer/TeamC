/**
* 8-2_oncerun.c
* > author: PhoenixXC
* > email: xuancbm@xiyoulinux.org
* > description: oncerun.c
* > created: Mon Aug 06 2018 09:58:03 GMT+0800 (CST)
* > last-modified: Mon Aug 06 2018 09:58:03 GMT+0800 (CST)
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

// pthread_once_t once = PTHREAD_ONCE_INIT;
pthread_once_t once = 0;

// 使用初值为PTHREAD_ONCE_INIT的once_control变量保证
// 函数在本进程执行序列中仅执行一次。

void run(void)
{
    printf("Function run is running in thread %u\n", pthread_self());
}

void * thread1(void * arg)
{
    pthread_t thid = pthread_self();
    printf("[1] Current thread's  ID is %u\n", thid);
    pthread_once(&once, run);
    printf("thread1 ends\n");
}

void * thread2(void * arg)
{
    pthread_t thid = pthread_self();
    printf("[2] Current thread's ID is %u\n", thid);
    pthread_once(&once, run);
    printf("thread2 ends\n");
}

int main()
{
    pthread_t thid1, thid2;

    pthread_create(&thid1, NULL, thread1, NULL);
    pthread_create(&thid2, NULL, thread2, NULL);
    sleep(3);
    printf("main thread exit!\n");
    exit(0);
}

/**
 * 在多线程编程环境下，尽管pthread_once()调用会出现在多个线程中，
 * init_routine()函数仅执行一次，究竟在哪个线程中执行是不定的，
 * 是由内核调度来决定。
 */
