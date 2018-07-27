/**
* 6-3_my_rwl.c
* @author: PhoenixXC
* @email: xuancbm@xiyoulinux.org
* @description: 文件读写和文件读写指针的移动操作
* @created: Tue Jul 24 2018 11:31:26 GMT+0800 (CST)
* @last-modified: Tue Jul 24 2018 11:31:26 GMT+0800 (CST)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

// 自定义的错误处理函数
void my_err(const char * err_string, int line)
{
    fprintf(stderr, "line:%d ", line);
    perror(err_string);
    exit(1);
}

// 自定义的读数据函数
int my_read(int fd)
{
    int     len;
    int     ret;
    int     i;
    char    read_buf[64];

    // 获取文件长度并保持文件读写指针在文件开始处
    if (lseek(fd, 0, SEEK_END) == -1)
        my_err("lseek", __LINE__);
    // __LINE__  编译器内置宏，表示行数
    // 文件读写指针置于文件尾， 获取文件头到文件尾的字节数
    if ((len = lseek(fd, 0, SEEK_CUR)) == -1)
        my_err("lseek", __LINE__);
    // 文件读写指针置于文件头
    if ((lseek(fd, 0, SEEK_SET)) == -1)
        my_err("lseek", __LINE__);
    // 输出文件长度
    printf("len: %d\n", len);

    // 读数据
    if ((ret = read(fd, read_buf, len)) < 0)
        my_err("read", __LINE__);
    
    // 打印数据
    for (i = 0; i < len; i++)
        printf("%c", read_buf[i]);
    printf("\n");

    // 返回实际读取文件内容的字节数
    return ret;
}

int main()
{
    int fd;
    char write_buf[32] = "Hello World";

    // 在当前目录下创建文件 example_63.c 
    // if ((fd = creat("example_63.c", S_IRWXU)) == -1)
    // ============================================
    // creat() 只能以 只写 的方式打开创建的文件，会导致 
    // create file success
    // len: 11
    // line:50 read: Bad file descriptor
    // ============================================
    // 可读可写、文件不存在自动创建、文件若存在则将文件长度清零  |  设置为对所有者可写可读
    if ((fd = open("example_63.c", O_RDWR | O_CREAT | O_TRUNC , S_IRWXU)) == -1)
        my_err("open", __LINE__);
    else
        printf("create file success\n");

    // 把 write_buf 里面的内容写入文件中, 没有写入空字符（strlen）
    if (write(fd, write_buf, strlen(write_buf)) != strlen(write_buf))
        my_err("write", __LINE__);

    my_read(fd);

    // 演示文件的间隔
    printf("/*---------------------*/\n");
    if (lseek(fd, 10, SEEK_END) == -1)
        my_err("lseek", __LINE__);
    if (write(fd, write_buf, strlen(write_buf)) != strlen(write_buf))
        my_err("write", __LINE__);

    my_read(fd);

    close(fd);
    return 0;
}
