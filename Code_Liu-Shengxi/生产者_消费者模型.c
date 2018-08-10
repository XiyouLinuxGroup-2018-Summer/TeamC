/*************************************************************************
	> File Name: produce_consume.c
	> Author: Liu Shengxi 
	> Mail: 13689209566@163.com
	> Created Time: 2018年08月10日 星期五 16时07分22秒
 ************************************************************************/

#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t notfull = PTHREAD_COND_INITIALIZER;  //不满
pthread_cond_t notempty = PTHREAD_COND_INITIALIZER; //不空

int TT = 0;
int flag = 1;

void *produce(void *arg) // return  1
{
	while(flag)
	{
		pthread_mutex_lock(&mutex);
		while (TT == 100) // 100
		{
			printf("full! producer is waiting\n");
			pthread_cond_wait(&notfull, &mutex); //等待不满
		}
		TT++;
		printf("我是生产者，我让TT+1,TT现在是%d\n", TT);
		pthread_cond_signal(&notempty); //发出队非空的消息
		pthread_mutex_unlock(&mutex);
	}
	pthread_exit(NULL);
	return (void *)1;
}
void *consume(void *arg) // return  2
{
	while(flag)
	{
		pthread_mutex_lock(&mutex);
		while (TT == 0)
		{
			printf("empty! consumer is waiting\n");
			pthread_cond_wait(&notempty, &mutex); //等待队不空
		}
		TT--;
		printf("我是消费者,我让TT-1,TT现在是%d\n", TT);

		pthread_cond_signal(&notfull); //发出队不满的消息
		pthread_mutex_unlock(&mutex);
	}
	pthread_exit(NULL);
	return (void *)2;
}
int main(void)
{
	pthread_t th1 = 0, th2 = 0;
	int ret1 = 0, ret2 = 0;

	int p1 = pthread_create(&th1, NULL, produce, NULL);
	int p2 = pthread_create(&th2, NULL, consume, NULL);

	printf("th1 == %d ,p1 == %d \n", th1, p1);
	printf("         th2 == %d ,p2 == %d \n", th2, p2);

	sleep(1);
	flag = 0 ;

	int tmp1 = pthread_join(th1, NULL);
	int tmp2 = pthread_join(th2, NULL);

	return 0;
}
