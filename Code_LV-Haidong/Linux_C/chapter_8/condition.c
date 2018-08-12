#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<pthread.h>
#include<string.h>

pthread_mutex_t mutex;//互斥锁
pthread_cond_t cond;//操作的条件变量

void * thread1(void * arg)
{
    pthread_cleanup_push (pthread_mutex_unlock,&mutex);//用于自动释放资源，pthread_clean_up_push()的调用点和pthread_clean_pop()的调用点

    while(1)
    {
        printf("thread1 is running\n");
        pthread_mutex_lock (&mutex);//建立互斥锁,
        //无论那种类型的锁，使用pthread_mutex_lock加锁时候，如果mutex被锁住，目前尝试加锁的线程就会阻塞,
        //其中一个必须等待解索，等待队列中的第一个线程将会获得互斥锁
        pthread_cond_wait (&cond,&mutex);//释放mutex指向的互斥锁，同时让当前线程关于cond指向的条件变量阻塞，直到条件被信号唤醒
        //线程改变条件变量的值，条件变量会获得一个信号，使得等待条件变量的线程退出阻塞状态
        printf ("thread1 applied the condition\n");
        pthread_mutex_unlock(&mutex);//将互斥锁解锁
        sleep(4);
    }
    pthread_cleanup_pop(0);
}

void *thread2(void * arg)
{
       while(1)
       {
           printf("thread2 is running\n");
           pthread_mutex_lock(&mutex);//实行互斥锁
           pthread_cond_wait(&cond,&mutex);//防止多个线程请求
           //基于条件等待，释放互斥锁
           printf("thread2 applied the condition\n");
           pthread_mutex_unlock(&mutex);//
           sleep(1);
       }
}

int main(void)
{
     pthread_t tid1,tid2;

     printf("condition variable study!\n");
     pthread_mutex_init(&mutex,NULL);
     pthread_cond_init(&cond,NULL);
     pthread_create(&tid1,NULL,(void *)thread1,NULL);
     pthread_create(&tid2,NULL,(void *)thread2,NULL);
   
    do{
        pthread_cond_signal(&cond);
    }while(1);
    sleep(10);
    pthread_exit(0);
}