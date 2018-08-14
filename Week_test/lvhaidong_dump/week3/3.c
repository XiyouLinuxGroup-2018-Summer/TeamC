#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>
#include<string.h>
//sleep的头文件在unisted.h中
pthread_cond_t cond;
pthread_mutex_t mutex;
int flag=0;
void * threadA(void *arg);
void * threadB(void *arg);
void * threadC(void *arg);
void * threadA(void *arg)
{
    while(1)
    {
        if(flag == 0)
       {
            pthread_t thid2;
            pthread_mutex_lock(&mutex);//加锁
            flag = 1;
            printf("A");  
            fflush(stdout);
            pthread_cond_signal(&cond);//发出信号
            pthread_cond_wait(&cond,&mutex); //沉睡,条件变量可以让我们沉睡到某种状态只有获得信号之后才会退出阻塞
            //cond释放互斥锁，同时让cond阻塞
            pthread_mutex_unlock(&mutex);  
            sleep(1);
       }
    }
}
void *threadB(void * arg)
{
    while(1)
    {
      if(flag == 1)
      {    
        pthread_mutex_lock(&mutex);//加锁
         flag = 2;
        printf("B");
        fflush(stdout);   
        pthread_cond_signal(&cond);  //发出信号
        pthread_cond_wait(&cond,&mutex);//沉睡，条件变量可以让我们沉睡到某种状态只有获得信号之后才会退出阻塞
        pthread_mutex_unlock(&mutex); 
        sleep(1);
      }
    }
}
void * threadC(void * arg)
{
    while(1)
     {
         if(flag == 2)
        {
            pthread_mutex_lock(&mutex);
              flag = 0;
            printf("C");
            fflush(stdout);
            pthread_cond_signal(&cond);
            pthread_cond_wait(&cond,&mutex);
            pthread_mutex_unlock(&mutex);
            sleep(1); 
        }
     }
}
int main()
{
    pthread_t thid1,thid2,thid3;
    pthread_mutex_init(&mutex,NULL);
    pthread_cond_init(&cond,NULL);
    pthread_create(&thid1,NULL,threadA,NULL);
    pthread_create(&thid2,NULL,threadB,NULL);
    pthread_create(&thid3,NULL,threadC,NULL);
    sleep(50);//父进程等待子进程的结束
    pthread_cond_destroy(&cond);//清除条件变量
    pthread_mutex_destroy(&mutex);
    pthread_exit(0);
}
