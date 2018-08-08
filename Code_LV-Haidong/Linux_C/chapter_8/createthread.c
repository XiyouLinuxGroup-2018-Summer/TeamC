#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>//创建线程所需的头文件

int * thread(void * arg)//线程所调用的函数
{
     pthread_t newthid;

     newthid = pthread_self();
     printf("this is a new thread , thread  ID = %u \n",newthid);
     return NULL;
}

int main(void)
{
    pthread_t thid;
    printf("main thread , ID is %u \n",pthread_self());///打印主线程的ID
    if(pthread_create(&thid,NULL,(void *)thread,NULL)!=0)
    {
        exit(1);
    }
    sleep(1);//让主线程等待子线程结束，不然无法打印相应的值
    return 0;
}
