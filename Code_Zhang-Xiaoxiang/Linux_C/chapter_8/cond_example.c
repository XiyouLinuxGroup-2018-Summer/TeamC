#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;      //全局变量的mutex可以通过这种方式初始化
pthread_cond_t  cond  = PTHREAD_COND_INITIALIZER;       //全局变量的cond可以通过这种方式初始化

struct node     //链表
{
    struct node *next;
    int counter;
};

struct node *head = NULL;

void *producter(void *arg)      //生产者
{
    struct node *p = NULL;
    while(1)
    {
        p = (struct node *)malloc(sizeof(struct node)); //生产者
        p->counter = rand() % 1000 + 1; //随即取一个值，作为生产的产品
        p->next = NULL;

        pthread_mutex_lock(&mutex); //上锁，如果不上锁，可能导致链表出现问题
        p->next = head;
        head = p;
        pthread_mutex_unlock(&mutex);   //解锁
        printf("producter:I has produced a product named %d\n", p->counter);
        pthread_cond_signal(&cond); //由于已经生产了一个产品，所以可以唤醒某个消费者了
        // sleep(rand() % 5);      //为了便于观察，所以添加睡眠
    }
}

void * consumer(void *arg)   //消费者
{
    struct node *p = NULL;
    while(1)
    {
        pthread_mutex_lock(&mutex); //上锁
        while(head == NULL)  
        {
            pthread_cond_wait(&cond, &mutex);   
        }
        p = head;
        head = p->next;     //模拟消费一个产品
        pthread_mutex_unlock(&mutex);       //解锁

        printf("consumer:I has taken a product named %d\n", p->counter);
        free(p);
        // sleep(rand() % 5);
    }
}
int main()
{
    pthread_t tid[2];

    pthread_create(&tid[0], NULL, producter, NULL);
    pthread_create(&tid[1], NULL, consumer, NULL);

    sleep(1);
    return 0;
    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);

    return 0;

}
