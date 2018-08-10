/*
    创建两个线程（称之为线程 A 和 线程 B） 和一个文件， 文件的最大存储数
    据量为 100 字节，初始数据为空。线程 A 负责向文件中写入数据（ 写入的
    数据自定），线程 B 负责从文件中读取数据。
    要求：
       ① 线程 A 不可向已满的文件中写入数据或写入数据使之超过 100 字节；
       ② 线程 B 不可从空文件中读取数据且读取量不可大于数据量；
       ③ 读取过的数据应从文件中删除；
       ④ 程序执行时不出现任何错误。
    假定每次读1个字节，写1个字节
*/

#include <stdio.h>
#include <fcntl.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#define maxsize 100                                     // 界定最大读写范围
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;      // 创建锁
pthread_cond_t  cond  = PTHREAD_COND_INITIALIZER;       // 创建条件变量
char filename[256] = "./temp.txt";                      // 临时文件名
int fd, flag = 1, count = 0;                                // 打开的文件id/状态标志

void myerr(char * errstring, int line)
{
    fprintf(stderr, "line: %d", line);
    perror(errstring);
    pthread_exit((void*)1);
}

void ThreadWrite(void * arg)
{
    char wBuf[2];
    int  wLenbytes, ch;                               // 文件指针位置
    srand((unsigned int)(time(NULL)));
    while(flag)
    {
        pthread_mutex_lock(&mutex);

        while (count == 100)
        {
            pthread_cond_wait(&cond, &mutex);
        }

        if((wLenbytes = lseek(fd, 0, SEEK_END)) < 0)     // 获取文件数据大小
            myerr("lseek", __LINE__);                    // 文件指针指向末尾以写数据

        if (wLenbytes < 100)
        {
            ch = rand() % 58;
            while ((ch < 48 || ch > 57))
                ch = rand() % 58;
            wBuf[0] = (char)ch;
            wBuf[1] = '\0';
            if (write(fd, wBuf, 1) < 0)
                myerr("write", __LINE__);
            printf("> Write content: %c\n", wBuf[0]);
            count = wLenbytes + 1;
        }

        pthread_mutex_unlock(&mutex);
        if (count)
            pthread_cond_signal(&cond);
        if (!flag)
            pthread_exit(0);
        sleep(rand() % 2);
    }
    pthread_exit(0);
}

void ThreadRead(void * arg)
{
    char rBuf[256];                                      // 读入数据
    int  rLenbytes;
    memset(rBuf, '\0', sizeof(rBuf));
    while(flag)
    {
        pthread_mutex_lock(&mutex);
        while (count == 0)
        {
            pthread_cond_wait(&cond, &mutex);
        }

        if((rLenbytes = lseek(fd, 0, SEEK_END)) < 0)     // 获取文件数据大小
            myerr("lseek", __LINE__);
        lseek(fd, 0, SEEK_SET);                          // 文件内容指针置于开始
        
        if (rLenbytes > 0)                               // 如果文件中有内容
        {
            if (read(fd, rBuf, rLenbytes) < 0)           // 读入数据
                myerr("read", __LINE__);
            rBuf[rLenbytes + 1] = '\0';
            if (ftruncate(fd, 0) < 0)                    // 清空文件
                myerr("ftruncate", __LINE__);
            lseek(fd, 0, SEEK_SET);
    
            if (write(fd, rBuf + 1, rLenbytes - 1) < 0)
                myerr("write", __LINE__);
            count--;
            pthread_mutex_unlock(&mutex);
            pthread_cond_signal(&cond);
            printf("> Read  content: %c\n", rBuf[0]);
            // printf("After Read file content :\n%s\n", rBuf);
            // printf("Now file len = %d\n", rLenbytes - 1);
            // printf("----------------------------------\n");
        }
        else
            pthread_mutex_unlock(&mutex);
        memset(rBuf, '\0', sizeof(rBuf));
        sleep(rand() % 3);
        if (!flag)
            pthread_exit(0);
    }
    pthread_exit(0);
}


int main(void)
{
    pthread_t thdA, thdB;
    fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, 0644);      // 打开/新建文件

    pthread_create(&thdB, NULL, (void*)ThreadRead, NULL);
    pthread_create(&thdA, NULL, (void*)ThreadWrite, NULL);

    sleep(10);               // 主线程沉睡
    flag = 0;               // 设置条件使线程结束

    // 善后
    pthread_join(thdA, NULL);
    pthread_join(thdB, NULL);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
    close(fd);
    printf("everything are done\n");
    return 0;
}
