#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

void assisthread(void *arg)
{
    printf("I am helping to do some jobs\n");
    //sleep(3);在这里有没有slepp都是一样的
    pthread_exit(0);//有两种特殊情况需要考虑
    //在主线程中，如果从main函数返回或者调用了exit函数退出主线程，则整个进程结束，进程结束之后，进程呢观众的线程也随之结束
    //如果主线程调用了pthread_exit函数，仅仅是主线程消亡
}

int main()
{
    pthread_t assisthin;
    int status;

    pthread_create ( &assisthin,  NULL,  (void *)assisthread,  NULL);//第三个参数表示调用的函数,第四个表示传入的参数
    pthread_join ( assisthin,  (void *)&status);
    printf("assistthread exit is caused %d\n",status);
   
    return 0;
}