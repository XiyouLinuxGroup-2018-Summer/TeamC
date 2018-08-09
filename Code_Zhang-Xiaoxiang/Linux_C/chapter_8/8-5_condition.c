/**
* 8-5_conditionc
* > author: PhoenixXC
* > email: xuancbm@xiyoulinux.org
* > description: 
* > created: Mon Aug 06 2018 20:57:47 GMT+0800 (CST)
* > last-modified: Mon Aug 06 2018 20:57:47 GMT+0800 (CST)
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

pthread_mutex_t mutex;             // 锁
pthread_cond_t  cond;              // 条件变量

void * thread1(void * arg)
{
    pthread_cleanup_push(pthread_mutex_unlock, &mutex);         // 自动释放资源

    while(1)
    {
        printf("thread1 is running\n");
        pthread_mutex_lock(&mutex);
        pthread_cond_wait(&cond, &mutex);
        printf("thread1 applied the condition\n");
        pthread_mutex_unlock(&mutex);
        sleep(4);
    }

    pthread_cleanup_pop(0);                                     // 自动释放资源
    // pthread_cleanup_push 和　pthread_cleanup_pop　提供回调函数保护
}

void * thread2(void * arg)
{
    while(1)
    {
        printf("thread2 is running\n");
        pthread_mutex_lock(&mutex);
        pthread_cond_wait(&cond, &mutex);
        printf("thread2 applied the condition\n");
        pthread_mutex_unlock(&mutex);
        sleep(1);
    }
}

int main(void)
{
    pthread_t tid1, tid2;

    printf("condition variable study!\n");
    pthread_mutex_init(&mutex, NULL);                       // 初始化互斥锁和条件变量
    pthread_cond_init(&cond, NULL);
    pthread_create(&tid1, NULL, (void*)thread1, NULL);      // 创建两个线程
    pthread_create(&tid2, NULL, (void*)thread2, NULL);  
    
    do{                                                     // 解除线程的阻塞
        pthread_cond_signal(&cond);
    }while(1);

    sleep(5);
    pthread_exit(0);  
    return 0;
}
