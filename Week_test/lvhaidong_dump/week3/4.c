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

int gflag=0;
int view=0,view1=0;
int temp1=0;
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
        
            pthread_mutex_lock (&mutex);
            FILE *fp;
            int fd,temp=0,len=0;                                                                                                                      
            char *write_buf;
            write_buf=(char *)malloc(30);
            fp=popen(" head -c 8 /dev/random | base64","r"); //调用系统函数生成随机字符串
            fgets(write_buf,8,fp); //调用系统函数生成随机字符串
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
            pthread_cond_wait(&cond,&mutex);
            printf("wirte  %s\n",write_buf);
            printf("----------------\n");
            temp = write(fd, write_buf,7);
            view=view+7;
            if(temp !=  7)
            {
                printf("error write");
                exit(1);
            }
            close(fd);
            free(write_buf);
            pthread_mutex_unlock(&mutex);
            sleep(4);
       
    }     
}
void * thread2(void * arg)
{
    while(1)
    {
             pthread_mutex_lock (&mutex);
            int fd=0,many=0;
            int ret=0,temp=0,len=0;
            char *red_buf,*restart_buf;
            red_buf=(char *)malloc(61);
            restart_buf=(char *)malloc(200);
            fd = open("Hello,Word.txt",O_RDONLY | O_CREAT );
            if(mylseek(fd)<0)
            {
                printf("文件已经没有内容");
                exit(1);
            }
          if(mylseek(fd) > 0)
          {
                if((mylseek(fd))>100 )
            {
                printf("文件超过容量，遇到错误");
                exit(1);
            }
            lseek(fd,view1,SEEK_SET);
            pthread_cond_wait(&cond,&mutex);
            ret = read(fd,red_buf,7);          
            printf("read %s\n",red_buf);
            printf("----------------\n");
            view1=view1+7;
            if(strlen(red_buf) > 100)
            {
                printf("error red_buf");
                exit(1);
            }
            close(fd);
            fd = open("Hello,Word.txt",O_RDWR | O_CREAT |O_APPEND,0664 );
            len=mylseek(fd);
            lseek(fd,view1,SEEK_SET);
            many=len-view1;//读取之后的长度
            ret = read(fd,restart_buf,many);
            printf("restart %s\n",restart_buf);
            printf("----------------\n");
            close(fd);
            fd = open("Hello,Word1.txt",O_RDWR | O_CREAT |O_TRUNC,0664 );
            temp1  = strlen(restart_buf);
            lseek(fd,0,SEEK_SET);
            write(fd,restart_buf,temp1);
            close(fd);
            view=many;//删除之后的长度
            view1=0;//从头开始读取
            gflag+=1;
            if(gflag>=1)
            {
                system("cp Hello,Word1.txt Hello,Word.txt");
                system("rm -rf Hello,Word1.txt");
            }
            free(red_buf);
            free(restart_buf);
             pthread_cond_wait(&cond,&mutex);
            pthread_mutex_unlock(&mutex);
            sleep(1);
          }
       
    }
}
int main()
{
    pthread_t thid1,thid2;
    pthread_mutex_init(&mutex,NULL);
    pthread_cond_init(&cond,NULL);
    //pthread_create(&thid1,NULL,thread1,NULL);
    pthread_create(&thid2,NULL,thread2,NULL);

    do{
        pthread_cond_signal (&cond);
    }while(1);
    sleep(50);
    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mutex);
    pthread_exit(0);
}