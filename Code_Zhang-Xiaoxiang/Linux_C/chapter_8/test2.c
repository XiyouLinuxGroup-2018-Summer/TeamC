#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
int myglobal;
pthread_mutex_t mymutex = PTHREAD_MUTEX_INITIALIZER;        // 初始化锁
void *thread_function(void *arg) 
{
    int i,j;
    for (i = 0; i < 20; i++) 
    {
        pthread_mutex_lock(&mymutex);
        j=myglobal;
        j=j+1;
        printf(".");
        fflush(stdout);
        // sleep(1);
        myglobal=j;
        pthread_mutex_unlock(&mymutex);
    }
    return NULL;
}
int main() 
{
    pthread_t mythread;
    int i;
    if(pthread_create(&mythread, NULL, thread_function, NULL)) 
    {
        printf("error creating thread.");
        abort();                        // 异常程序终止，同时发送SIGABRT信号给调用进程
    }
    for(i = 0; i < 20; i++) 
    {
        pthread_mutex_lock(&mymutex);
        myglobal=myglobal+1;
        pthread_mutex_unlock(&mymutex);
        printf("o");
        fflush(stdout);
        // sleep(1);
    }
    if(pthread_join( mythread, NULL)) 
    {
        printf("error joining thread.");
        abort();
    }
    printf("\nmyglobal equals %d\n",myglobal);
    exit(0);
}
