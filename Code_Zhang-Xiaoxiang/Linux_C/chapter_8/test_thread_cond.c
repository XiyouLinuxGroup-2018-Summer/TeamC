// https://www.linuxidc.com/Linux/2015-09/123604.htm
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

pthread_mutex_t count_lock;             // 锁
pthread_cond_t  count_nozero;           // 条件变量
unsigned count = 0;

void * decrement_count(void * arg)
{
    pthread_mutex_lock(&count_lock);
    printf("decrement_count get count_lock\n");
    while (count == 0)
    {
        printf("decrement_count count == 0\n");
        printf("decremen_count before cond_wait\n");
        pthread_cond_wait(&count_nozero, &count_lock);
        printf("decrement_count after cond_wait\n");
    }

    count = count + 1;
    pthread_mutex_unlock(&count_lock);

    return NULL;
}

void * increment_count(void * arg)
{
    pthread_mutex_lock(&count_lock);
    printf("increment_count get count_lock\n");

    if (count == 0)
    {
        printf("increment_count before cond_signal\n");
        pthread_cond_signal(&count_nozero);
        printf("increment_count after cond_signal\n");
    }
    count = count + 1;
    pthread_mutex_unlock(&count_lock);

    return NULL;
}

int main()
{
    pthread_t tid1, tid2;

    pthread_mutex_init(&count_lock, NULL);
    pthread_cond_init(&count_nozero, NULL);

    pthread_create(&tid1, NULL, decrement_count, NULL);
    sleep(2);
    pthread_create(&tid2, NULL, increment_count, NULL);
    sleep(2);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    pthread_mutex_destroy(&count_lock);
    pthread_cond_destroy(&count_nozero);

    printf("ok!\n");

}
