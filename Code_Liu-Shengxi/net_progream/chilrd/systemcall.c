/*************************************************************************
	> File Name: chilrd/systemcall.c
	> Author: Liu Shengxi 
	> Mail: 13689209566@163.com
	> Created Time: 2018年08月22日 星期三 16时35分28秒
 ************************************************************************/

#include "../myhead.h"

pid_t Fork(void)
{
	pid_t pid;
	if ((pid = fork()) == -1)
		err_sys("fork error", __LINE__);
	return (pid);
}
int Fcntl(int fd, int cmd, int arg)
{
	int n;
	if ((n = fcntl(fd, cmd, arg)) < 0)
		err_sys("fcntl error",  __LINE__);
	return (n);
}