/*************************************************************************
	> File Name: chilrd/socket.c
	> Author: Liu Shengxi 
	> Mail: 13689209566@163.com
	> Created Time: 2018年08月22日 星期三 16时17分08秒
 ************************************************************************/

#include "../myhead.h"

int err_sys(const char *err_string, int line)
{
	fprintf(stderr, "Error At line : %d \n", line); // 显示出错行
	perror(err_string);								// err_string + 出错信息
	exit(-1);
}

int Socket(int family, int type, int protocol)
{
	int n;
	if ((n = socket(family, type, protocol)) < 0)
		err_sys("socket error ", __LINE__);
	return (n);
}
int Bind(int fd, const struct sockaddr *sa, socklen_t salen)
{
	if (bind(fd, sa, salen) < 0)
		err_sys("bind error", __LINE__);
}
void Listen(int fd, int backlog)
{
	char *ptr = NULL;
	if ((ptr = getenv("LISTENQ")) != NULL)
		backlog = atoi(ptr);

	if (listen(fd, backlog) < 0)
		err_sys("listen error", __LINE__);
}
int Accept(int fd, struct sockaddr *sa, socklen_t *salenptr)
{
	int n;
again:
	if ((n = accept(fd, sa, salenptr)) < 0)
	{
#ifdef EPROTO
		if (errno == EPROTO || errno == ECONNABORTED)
#else
		if (errno == ECONNABORTED)
#endif
			goto again;
		else
			err_sys("accept error", __LINE__);
	}
	return (n);
}
void Close(int fd)
{
	if (close(fd) == -1)
		err_sys("close error", __LINE__);
}
/*代查*/
ssize_t Sendlen(int fd, const void *buf, size_t len, int flags)
{
	ssize_t n = 0;
	size_t sum = 0;
	const char *ptr;
	ptr = (const char *)buf;
	while (sum < len)
	{
		n = send(fd, (void *)ptr, len - sum, flags);
		if (n < 0)
		{
			if (errno == EINTR)
				n = 0;
			else
				err_sys("send error ", __LINE__);
		}
		sum += n;
		ptr += n;
	}
	return (sum);
}
ssize_t Recvlen(int fd, void *buf, size_t len, int flags) //待查
{
	ssize_t n = 0;
	size_t sum = 0;
	const char *ptr;
	ptr = buf;
	while (sum < len)
	{
		n = recv(fd, (void *)ptr, len - sum, flags);
		if (n < 0)
		{
			if (errno == EINTR)
				n = 0;
			else
				err_sys("recv error ", __LINE__);
		}
		else if (n == 0)
		{
			/*对端关闭连接了*/
			printf("对端连接关闭！！！\n");
			break;
		}
		sum += n;
		ptr += n;
	}
	return (sum);
}

/*代查*/
int Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
		   struct timeval *timeout)
{
	int n;

	if ((n = select(nfds, readfds, writefds, exceptfds, timeout)) < 0)
		err_sys("select error", __LINE__);
	return (n); /* can return 0 on timeout */
}
