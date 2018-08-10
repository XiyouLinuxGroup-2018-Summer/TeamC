/**
    编写一个至少具有三个线程的程序（ 称之为线程 A、 B 和 C）， 其中线程 A
    输出字符'A'， 线程 B 输出字符'B'， 线程 C 输出字符'C'。 使得最终输出
    结果为“ ABCABCABC……”。
 **/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;          // 初始化锁
pthread_cond_t  cond  = PTHREAD_COND_INITIALIZER;           // 初始化条件变量
int button0 = 1, button1 = 0, button2 = 0;                  // 条件
int boolt = 1;

void Thread0(void * arg)
{
    while (boolt)
    {
        pthread_mutex_lock(&mutex);
        while (button0 && boolt)
        {            
            printf("A");
            button0 = 0;
            button1 = 1;
            fflush(stdout);
            sleep(1);
            pthread_cond_signal(&cond);
            if (boolt)
                pthread_cond_wait(&cond, &mutex);
        }
        pthread_mutex_unlock(&mutex);
    }
    pthread_exit(0);
}

void Thread1(void * arg)
{
    while (boolt)
    {    
        pthread_mutex_lock(&mutex);
        while (button1 && boolt)
        {
            printf("B");
            button1 = 0;
            button2 = 1;
            fflush(stdout);
            sleep(1);
            pthread_cond_signal(&cond);
            if (boolt) 
                pthread_cond_wait(&cond, &mutex);
        }
        pthread_mutex_unlock(&mutex);
    }
    pthread_exit(0);
}

void Thread2(void * arg)
{
    while(boolt)
    {
        pthread_mutex_lock(&mutex);
        while (button2 && boolt)
        {
            printf("C");
            button2 = 0;
            button0 = 1;
            fflush(stdout);
            sleep(1);
            pthread_cond_signal(&cond);
            if (boolt)
                pthread_cond_wait(&cond, &mutex);
        }
        pthread_mutex_unlock(&mutex);
    }
    pthread_exit(0);
}

int main(void)
{
    pthread_t thd0, thd1, thd2;
    pthread_create(&thd0, NULL, (void*)Thread0, NULL);
    pthread_create(&thd1, NULL, (void*)Thread1, NULL);
    pthread_create(&thd2, NULL, (void*)Thread2, NULL);

    // 等待并清理线程
    sleep(7);
    boolt = 0;

    printf("\nCancel work done!\n");
    pthread_join(thd0, NULL);
    pthread_join(thd1, NULL);
    pthread_join(thd2, NULL);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    return 0;
}
