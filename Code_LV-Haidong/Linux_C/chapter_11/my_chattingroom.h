#pragma once
#ifndef _MY_CHATTINGROOM_H
#define _MY_CHATTINGROOM_H

#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include <mysql/mysql.h>
#include <sys/epoll.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include <termios.h>
#include <ncurses.h>
#include <locale.h>
#include <assert.h>



int getch1(void);

#define BACKSPACE 127
#define MY_PASSWD Lhd013012?

int getch2(void)
{
    struct termios tm, tm_old;
    int fd = STDIN_FILENO, c;
    if (tcgetattr(fd, &tm) < 0)
        return -1;
    tm_old = tm;
    cfmakeraw(&tm);
    if (tcsetattr(fd, TCSANOW, &tm) < 0)
        return -1;
    c = fgetc(stdin);
    if (tcsetattr(fd, TCSANOW, &tm_old) < 0)
        return -1;
    return c;
}

typedef struct GROUP
{
  char number[13];//成员的账号
  int state;//状态区别管理员和群主 
}group;
typedef struct QQ//QQ的状态
{
    char number[13];//账号密码
    char passwd[13];
    char username[32];//用户名称
    int state;//表示有没有登陆登陆状态   3为群主
    int flag;//验证注册情况
    char qusetion[100];//密保问题
    char answer[100];//密保答案
    char to[13];
    char record[130];
    int size;
    struct QQ *next;
} qq;

struct  chatting
{
   char QQ[13];
   char username[32];
   int send_conn_fd;//套接字
   int flag;
   int state;//黑名单,特别关心
   struct chatting * next;
};

typedef struct INPUT
{
    int size;//发送的文件长度
    char intput_record[200];
}input;


void my_time()
{
    time_t tmpcal_ptr;
    struct tm *tmp_ptr = NULL;
    tmp_ptr = localtime(&tmpcal_ptr);
	printf ("after localtime, the time is:%d.%d.%d ", (1900+tmp_ptr->tm_year), (1+tmp_ptr->tm_mon), tmp_ptr->tm_mday);
	printf("%d:%d:%d\n", tmp_ptr->tm_hour, tmp_ptr->tm_min, tmp_ptr->tm_sec);
}


void my_err(const char * err_string,int line)
{
    fprintf(stderr,"line %d",line);
    perror("err_string");
    exit(1);
}

#endif