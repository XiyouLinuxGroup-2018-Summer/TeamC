/*************************************************************************
	> File Name: Recvline.c
	> Author: Liu Shengxi 
	> Mail: 13689209566@163.com
	> Created Time: 2018年08月23日 星期四 14时18分27秒
 ************************************************************************/

#include "../myhead.h"

static int recv_cnt = 0;
static char *recv_ptr = NULL;
static char recv_buf[MAXLINE];

static ssize_t my_recv(int fd, char *ptr, int flags)
{
	if (recv_cnt <= 0)
	{
	again:
		if ((recv_cnt = recv(fd, recv_buf, sizeof(recv_buf), flags)) < 0)
		{
			if (errno == EINTR)
				goto again;
			else
				return (-1);
		}
		else if (recv_cnt == 0)
		{
			return (0);
		}
		recv_ptr = recv_buf;
	}
	recv_cnt--;
	*ptr = *recv_ptr++;
	return (1);
}
ssize_t recvline(int fd, void *vptr, size_t maxlen, int flags)
{
	ssize_t n, rc;
	char c, *ptr;
	ptr = vptr;
	for (n = 1; n < maxlen; n++)
	{
		if ((rc = my_recv(fd, &c, flags)) == 1)
		{
			*ptr++ = c;
			if (c == '\n')
				break;
		}
		else if (rc == 0)
		{
			*ptr = 0;
			return (n - 1);
		}
		else
			return (-1);
	}
	*ptr = 0;
	return (n);
}
ssize_t Recvline(int fd, void *buf, size_t Maxlen, int flags)
{ //注意参数 Maxlen
	ssize_t n;
	if ((n = recvline(fd, buf, Maxlen, flags)) < 0)
		err_sys("recvline error ", __LINE__);
	return (n);
}
