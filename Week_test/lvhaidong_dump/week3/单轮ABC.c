#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>
//sleep的头文件在unisted.h中
pthread_key_t key;

void * threadA(void *arg)
{
    char * threadAname="A";
    pthread_setspecific(key,threadAname);
    printf("%s",pthread_getspecific(key));
}
void *threadB(void * arg)
{
    char * tsd="B";
    pthread_setspecific(key,tsd);
    printf("%s",pthread_getspecific(key));
}
void * threadC(void * arg)
{
     char * tsd="C";
     pthread_setspecific(key,tsd);
     printf("%s",pthread_getspecific(key));
}
int main()
{
    pthread_t thid1,thid2,thid3;
    pthread_key_create(&key,NULL);
    int i;
    for(i=1;i<=3;i++) 
    { 
        pthread_create(&thid1,NULL,threadA,NULL);
        sleep(1);
        pthread_create(&thid2,NULL,threadB,NULL);
        sleep(1);
        pthread_create(&thid3,NULL,threadC,NULL);
        sleep(2);
    }
    printf("\n");
    pthread_key_delete(key);
    return 0;
    return 0;
}
