#define MY_RECV_C

#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<errno.h>
#include"my_recv.h"


//自定义的错误处理函数
void my_err(const char * err_string,int line)
{
    fprintf(stderr,"line : %d",line);
    perror(err_string);
    exit(1);
}
/*
conn_fd  从该连接套接字上接受数据
data_buf-- 读取到的数据保存到此缓冲区中
len ----data_buf 所指向的空间长度*/
int my_recv(int conn_fd,char * data_buf,int len)
{
       static char recv_buf[BUFSIZE];
       static char * pread;//指向下次读取数据的位置
       static int len_remain = 0; //自定义缓冲区的剩余字节
       int i;
       //如果自定义缓冲区中没有数据，则从套接字中读取数据
       if(len_remain <= 0)
       {
           if((len_remain = recv(conn_fd,recv_buf,sizeof(recv_buf),0))<0)
           {
               perror("recv");
           }
           else if(len_remain  == 0)
           {
               return 0;
           }
           pread = recv_buf;//重新初始化指针
       }
       //从自定义缓冲区中读取了一次数据
       for(i=0;*pread != '\n';i++)
       {
              if(i>len)//防止指针越界
              {
                  return -1;
              }
              data_buf[i]=*pread++;
              len_remain--;
       }
       //去除结束标志
       len_remain--;
       pread++;

       return i;//读取成功
}



















