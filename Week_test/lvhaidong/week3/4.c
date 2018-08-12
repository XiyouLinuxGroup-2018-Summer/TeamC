#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<pthread.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>

pthread_mutex_t mutex;
pthread_cond_t  cond;

int flag=0;
int view=0;
int gflag=0;
int  mylseek(int fd)
{
     int len;
     len=lseek(fd,0,SEEK_END);//从文件头移动到尾巴
     if(len == -1)
     {
         printf("error lseek");
     }
     return len;
}
void * thread1(void * arg)//写的进程
{
     while(1)
    {
        if(flag == 0)
        { 
            pthread_mutex_lock (&mutex);
            flag = 1;
            pthread_cond_signal(&cond);
            FILE *fp;
            int fd,temp,len=0;                                                                                                                      
            char *write_buf;
            write_buf=(char *)malloc(30);
            fp=popen(" head -c 10 /dev/random | base64","r"); //调用系统函数生成随机字符串
            fgets(write_buf,10,fp); //调用系统函数生成随机字符串
            printf("wirte  %s\n",write_buf);
            if(gflag>=1)
                system("cp Hello,Word1.txt Hello,Word.txt");
            gflag+=1;
            fd = open("Hello,Word.txt",O_RDWR | O_CREAT | O_APPEND,0664);
            if(fd == -1)
            {
                printf("open error");
            }
            if(strlen(write_buf) > 100)
            {
                printf("error too big ");
                exit(1);
            }
            if(mylseek(fd)+strlen(write_buf) > 100)
            {
                printf("error 已经没有写的空间了");
                exit(1);
            }
            if((len=mylseek(fd) )> 100)
            {
                printf("error 已经满了");
                exit(1);
            }
             printf("now  %d\n",len);
            lseek(fd,view,SEEK_SET);
            temp = write(fd, write_buf,10);
            if(temp !=  10)
            {
                printf("error write");
                exit(1);
            }
            close(fd);
            free(write_buf);
            pthread_cond_wait(&cond,&mutex);
            pthread_mutex_unlock(&mutex);
            sleep(2);
       }
    }     
}
void * thread2(void * arg)
{
    while(1)
    {
        if(flag == 1)
       {
            pthread_mutex_lock (&mutex);
            flag = 0;
            pthread_cond_signal(&cond);
            int fd;
            int ret,temp;
            char *red_buf,*restart_buf;
            red_buf=(char *)malloc(61);
            restart_buf=(char *)malloc(200);
            memset(red_buf,0,sizeof(red_buf));
            memset(restart_buf,0,sizeof(restart_buf));
            fd = open("Hello,Word.txt",O_RDONLY | O_CREAT );
            if(mylseek(fd)>100)
            {
                printf("文件超过容量，遇到错误");
                exit(1);
            }
            temp=strlen(red_buf);
            lseek(fd,view,SEEK_SET);
            ret = read(fd,red_buf,6);
            printf("read %s\n",red_buf);
            if(strlen(red_buf) > 100)
            {
                printf("error red_buf");
                exit(1);
            }
            lseek(fd,6+view,SEEK_SET);
            ret = read(fd,restart_buf,4);
            view=view+2;
            printf("restart %s\n",restart_buf);
            close(fd);
            fd = open("Hello,Word1.txt",O_RDWR | O_CREAT |O_APPEND,0664 );
            lseek(fd,view,SEEK_SET);
            write(fd,restart_buf,strlen(restart_buf));
            close(fd);
            free(red_buf);
            free(restart_buf);
            pthread_cond_wait(&cond,&mutex);
            pthread_mutex_unlock(&mutex);
            sleep(2);
       }
    }
}
int main()
{
    pthread_t thid1,thid2;
    pthread_mutex_init(&mutex,NULL);
    pthread_cond_init(&cond,NULL);
    pthread_create(&thid1,NULL,thread1,NULL);
    pthread_create(&thid2,NULL,thread2,NULL);
    sleep(50);
    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mutex);
    pthread_exit(0);
}