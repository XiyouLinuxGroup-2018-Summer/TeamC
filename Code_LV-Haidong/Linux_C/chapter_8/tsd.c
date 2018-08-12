#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>

pthread_key_t key;

void * thread2(void * arg)
{
    int tsd=5;
    printf("thread %d is running\n",pthread_self());//获取pid
    pthread_setspecific(key, (int *)tsd);//给予了一个特殊的建位
    printf("thread %d returns %d\n",pthread_self(),pthread_getspecific(key));//getspecific函数获取私有数据的键值
}
void * thread1(void * arg)
{
    int tsd=0;
//    pthread_t thid2;

    printf("thread %d is running\n",pthread_self());//获取pid
    pthread_setspecific(key,(int *)tsd);//给予特殊的建位这个建位为0

    sleep(1);
    printf("thread %d returns %d\n",pthread_self(),pthread_getspecific(key));
}

int main(void)
{
   pthread_t thid1,thid2;
   printf("main thread begins runnning \n");
   pthread_key_create (&key,NULL);//创建一个建值，第二个参数如果不为空的话，线程退出之后
   pthread_create (&thid1,NULL,thread1,NULL);//创建一个线程，同时调用thread1线程函数
       pthread_create (&thid2,NULL,thread2,NULL);
   sleep(3);
   pthread_key_delete (key);
   printf("main thread exit\n");
   return 0;
}