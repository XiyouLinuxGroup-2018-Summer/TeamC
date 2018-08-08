// https://blog.csdn.net/qq_33951180/article/details/72801228
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

int a=0;
int b=0;
pthread_mutex_t mutex_a;        // 锁a
pthread_mutex_t mutex_b;        // 锁b

void* another(void* arg)
{
    pthread_mutex_lock(&mutex_b);       // 对b加锁
    printf("new_thread,got mutex_b,waiting for mutex_a\n");
    sleep(5);
    ++b;
    pthread_mutex_lock(&mutex_a);       // 对a加锁
    b += a++;
    pthread_mutex_unlock(&mutex_a);     // 解锁a,b
    pthread_mutex_unlock(&mutex_b);
    pthread_exit(NULL);                 // 退出进程
}

int main()
{
    pthread_t id;
    pthread_mutex_init(&mutex_a, NULL);         // 分别初始化两把锁
    pthread_mutex_init(&mutex_b, NULL);
    pthread_create(&id, NULL, another, NULL);   // 创建线程

    pthread_mutex_lock(&mutex_a);               // 和线程中申请锁的顺序相反
    printf("main_thread,got mutex_a,waiting for mutex_b\n");
    sleep(5);
    ++a;
    pthread_mutex_lock(&mutex_b);               
    a += b++;
    pthread_mutex_unlock(&mutex_b);
    pthread_mutex_unlock(&mutex_a);

    pthread_join(id, NULL);
    pthread_mutex_destroy(&mutex_a);
    pthread_mutex_destroy(&mutex_b);
    return 0;
}

/*******************************************************************************************
死锁产生的条件 
1、互斥属性：即每次只能有一个线程占用资源。 
2、请求与保持：即已经申请到锁资源的线程可以继续申请。在这种情况下，一个线程也可以产生死锁情况，即抱着锁找锁。 
3、不可剥夺：线程已经得到所资源，在没有自己主动释放之前，不能被强行剥夺。 
4、循环等待：多个线程形成环路等待，每个线程都在等待相邻线程的锁资源。
死锁的避免： 
1、既然死锁的产生是由于使用了锁，那么在能不使用锁的情况下就尽量不使用，如果有多种方案都能实现，那么尽量不选用带锁的这种方案 
2、尽量避免同时获得多把锁，如果有必要，就要保证获得锁的顺序相同
********************************************************************************************/
