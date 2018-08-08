// https://blog.csdn.net/qq_33951180/article/details/72801228
// 多线程编程中，难免会遇到多个线程同时访问临界资源的问题，如果不对其加以保护，那么结果肯定是不如预期的
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;  // 互斥锁初始化
static int g_val=0;
void* pthread_mem(void* arg)
{
    int i=0;
    int val=0;
    while(i < 500000)                       // 
    {
        pthread_mutex_lock(&mutex);         // 加锁
        val = g_val;
        i++;
        g_val=val+1;        
        pthread_mutex_unlock(&mutex);       // 解锁
    }
    // 通过对临界区进行加锁与解锁，每个线程在进入临界区的时候获得一把锁，
    // 操作执行完成后释放锁资源，使得其他等待的线程能够抱锁进入，这样就
    // 确保了每个线程进来执行的操作都是原子的，这样使得最后的结果为1000000.
    return NULL;
}

int main()
{
    pthread_t tid1;
    pthread_t tid2;
    pthread_create(&tid1, NULL, pthread_mem, NULL);
    pthread_create(&tid2, NULL, pthread_mem, NULL);
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    printf("g_val end is :%d\n",g_val);
    // 最后的结果是随机的
    return 0;
}

// mutex的本质是一种变量。假设mutex为1时表示锁是空闲的，
// 此时某个进程如果调用lock函数就可以获得所资源；当mutex
// 为0时表示锁被其他进程占用，如果此时有进程调用lock来获得锁时会被挂起等待。
