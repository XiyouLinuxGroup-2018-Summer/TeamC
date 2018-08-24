/*************************************************************************
	> File Name: origin.h
	> Author: Liu Shengxi
	> Mail: 13689209566@163.com
	> Created Time: 2018年08月14日 星期二 15时17分38秒
 ************************************************************************/

#ifndef _ORIGIN_H
#define _ORIGIN_H
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <sys/select.h>
#include <strings.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>

#define MAXLINE 1024
#define SA struct sockaddr
#define SERV_PORT 9877
#define LISTENQ 1024
/* 错误处理函数 */
int err_sys(const char *err_string, int line);

/* 标准IO函数封装 */
void Fclose(FILE *);
//FILE *Fdopen(int, const char *);
char *Fgets(char *, int, FILE *);
FILE *Fopen(const char *, const char *);
void Fputs(const char *, FILE *);

/*基本网络编程函数封装*/
int Socket(int family, int type, int protocol);
int Bind(int fd, const struct sockaddr *sa, socklen_t salen);
void Listen(int fd, int backlog);
int Accept(int fd, struct sockaddr *sa, socklen_t *salenptr);
void Close(int fd);

/*封装send与recv函数之类的*/
ssize_t Sendlen(int fd, const void *buf, size_t len, int flags);
ssize_t Recvlen(int fd, void *buf, size_t len, int flags);
ssize_t Recvline(int fd, void *buf, size_t Maxlen, int flags); //注意 Maxlen 参数

// 其余IO函数
int Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
		   struct timeval *timeout);

/*基本系统调用（fork 等）的封装*/
pid_t Fork(void);
int Fcntl(int fd, int cmd, int arg);

#endif
