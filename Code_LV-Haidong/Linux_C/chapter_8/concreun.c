#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
pthread_once_t once = PTHREAD_ONCE_INIT;//限制只能被执行一次
//函数原型为int pthread_once(pthread_once_t *once_control,void(*init_routinr)(void))

void run()
{
    printf("Fuction run is runnning in thread %u \n",pthread_self());
}
void * thread1(void * arg)///线程函数
{
    pthread_t thid = pthread_self();
    printf("Current thread is ID is %u \n",thid);
    run();
   // pthread_once(&once,run);//调用线程函数，保证run函数在进程中只执行了一次
    printf("thread1 ends \n");
}
void * thread2(void * arg)///线程函数
{
    pthread_t thid = pthread_self();
    printf("Current thread is ID is %u \n",thid);
    run();
    //pthread_once(&once,run);
    printf("thread2 ends \n");
}
int main()
{
    pthread_t thid1,thid2;

    pthread_create(&thid1,NULL,thread1,NULL);//thid1 来存储创建的线程pid，第二个参数指定线程的属性
    pthread_create(&thid2,NULL,thread2,NULL);//thread2表示线程调用哪个函数,arg标识传递给线程函数的参数
    sleep(3);
    printf("main thread exit ! \n");
    exit(0);//在这里如果使用return 的话顺序会发生改变，return 表示函数执行完才返回s
}
