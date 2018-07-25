#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

//自定义的错误处理函数
void my_err(const char * err_string, int line)
{
    fprintf("stderr", "line:%d", line);
    perror(err_string);
    exit(1);
}

//自定义的读数据函数
int my_read(int fd)
{
    int len;
    int ret;
    int i;
    char read_buf[64];
    
    //获取文件长度并保持文件读写指针在文件开始处
    if(lseek(fd, 0, SEEK_END) == -1)
    {
        my_err("lseek", __LINE__);
    }
    if((len = lseek(fd, 0, SEEK_CUR)) == -1)
    {
        my_err("lseek", __LINE__);
    }
    if((lseek(fd, 0, SEEK_SET)) == -1)
    {
        my_err("lseek", __LINE__);
    }

    printf("len:%d\n", len);
    //读数据
    if((ret = read(fd, read_buf, len)) < 0)
    {
        my_err("read", __LINE__);
    }

    //打印数据
    for(i = 0; i < len; i++)
    {
        printf("%c", read_buf[i]);
    }
    printf("\n");

    return ret;
}

int main()
{
    int fd;
    char write_buf[32] = "Hello World!";

    //在当前目录下创建文件example_63.c
    //if((fd = creat("example_63.c", S_IRWXU)) == -1)
    if((fd = open("example_63.c", O_RDWR|O_CREAT|O_TRUNC, S_IRWXU)) == -1)
    {
        my_err("open", __LINE__);
    }
    else
    {
        printf("create file success\n");
    }

    //写数据
    if(write(fd, write_buf, strlen(write_buf)) != strlen(write_buf))
    {
        my_err("write", __LINE__);
    }
    my_read(fd);

    //演示文件的间隔
    printf("/*-------------------*/\n");
    if(lseek(fd, 10, SEEK_END) == -1)
    {
        my_err("lseek", __LINE__);
    }
    if(write(fd, write_buf, strlen(write_buf)) != strlen(write_buf))
    {
        my_err("write", __LINE__);
    }
    my_read(fd);
    close(fd);
    return 0;
}
