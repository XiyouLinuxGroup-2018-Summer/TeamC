/*************************************************************************
	> File Name: stdio.c
	> Author: Liu Shengxi 
	> Mail: 13689209566@163.com
	> Created Time: 2018年08月22日 星期三 16时06分36秒
 ************************************************************************/


#include "../myhead.h"
void Fclose(FILE *fp)
{
	if (fclose(fp) != 0)
		err_sys("fclose error",__LINE__);
}
char *
Fgets(char *ptr, int n, FILE *stream)
{
	char *rptr;

	if ((rptr = fgets(ptr, n, stream)) == NULL && ferror(stream))
		err_sys("fgets error", __LINE__);

	return (rptr);
}

FILE *
Fopen(const char *filename, const char *mode)
{
	FILE *fp;

	if ((fp = fopen(filename, mode)) == NULL)
		err_sys("fopen error", __LINE__);

	return (fp);
}

void Fputs(const char *ptr, FILE *stream)
{
	if (fputs(ptr, stream) == EOF)
		err_sys("fputs error", __LINE__);
}

