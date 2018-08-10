#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include <pthread.h>
#define MAX_THREAD 3 //线程个数
unsigned long long main_counter, counter[MAX_THREAD];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;      // 初始化锁
void * thread_worker(void * arg);

int main(void)
{
    int i, rtn, ch;
    int tmp[MAX_THREAD];
    pthread_t pthread_id[MAX_THREAD] = { 0 }; 
    
    // 用循环创建３个线程
    for (i = 0; i < MAX_THREAD; i++)
        pthread_create(&pthread_id[i], NULL, (void *)thread_worker, (void *)i);

    // 用户按一次回车执行下面的循环体一次,按q退出
    do
    {
        unsigned long long sum = 0;
        for (i = 0; i < MAX_THREAD; i++)
        {
            sum += counter[i];
            printf("%llu ", counter[i]);
        }
        printf("%llu/%llu", main_counter, sum);
    }while ((ch = getchar()) != 'q');

    return 0;
}

void * thread_worker(void * arg)
{
    int thread_num;
    thread_num = (int)arg;
    // 死循环
    for (; ;)
    {
        counter[thread_num]++;
        main_counter++;
    }
}

