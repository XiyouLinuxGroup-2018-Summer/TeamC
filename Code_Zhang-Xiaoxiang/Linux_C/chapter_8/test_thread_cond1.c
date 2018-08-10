#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

pthread_mutex_t counter_lock;
pthread_cond_t counter_nonzero;
int counter = 0;

void * decrement_counter(void * argv);
void * increment_counter(void * argv);

int main(void)
{
    printf("counter: %d\n", counter);
    pthread_mutex_init(&counter_lock, NULL);
    pthread_cond_init(&counter_nonzero, NULL);

    pthread_t thd1, thd2;
    int ret = -1;

    ret = pthread_create(&thd1, NULL, decrement_counter, NULL);
    if (ret)
    {
        printf("create thread1 fail\n");
        return -1;
    }
    ret = pthread_create(&thd2, NULL, increment_counter, NULL);
    if (ret)
    {
        printf("create thread2 fail\n");
        return -1;
    }

    int counter = 0;
    printf("the counter from main at: %p\n", &counter);
    while (counter != 10)
    {
        printf("counter(main): %d at %p\n", counter, &counter);
        sleep(1);
        counter++;
    }

    pthread_join(thd1, NULL);
    pthread_join(thd2, NULL);
    pthread_mutex_destroy(&counter_lock);
    pthread_cond_destroy(&counter_nonzero);

    printf("ok!\n");
}

void * decrement_counter(void * argv)
{
    printf("counter(decrement): %d\n", counter);
    pthread_mutex_lock(&counter_lock);
    while (counter == 0)
        pthread_cond_wait(&counter_nonzero, &counter_lock);
    printf("counter--(decrement, before): %d\n", counter);
    counter--;
    printf("counter--(decrement, after): %d\n", counter);
    pthread_mutex_unlock(&counter_lock);
    return NULL;
}


void * increment_counter(void * argv)
{
    printf("counter(increment): %d\n", counter);

    pthread_mutex_lock(&counter_lock);
    if (counter == 0)
        pthread_cond_signal(&counter_nonzero);

    printf("counter++(increment, before): %d\n", counter);
    counter++;
    printf("counter++(increment, after): %d\n", counter);
    pthread_mutex_unlock(&counter_lock);

    return NULL;
}
