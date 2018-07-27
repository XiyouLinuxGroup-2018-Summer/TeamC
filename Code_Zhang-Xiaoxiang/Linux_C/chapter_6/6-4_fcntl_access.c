/**
* 6-4_fcntl_access.c
* @author: PhoenixXC
* @email: xuancbm@xiyoulinux.org
* @description: fcntl() 使用 emmm
* @created: Tue Jul 24 2018 15:26:43 GMT+0800 (CST)
* @last-modified: Tue Jul 24 2018 15:26:43 GMT+0800 (CST)
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

// 错误处理
void my_err(const char * err_string, int line)
{
    fprintf(stderr, "line:%d ", line);
    perror(err_string);
    exit(1);
}

int main()
{
    int ret;
    int access_mode;
    int fd;

    if ((fd = open("example_64", O_CREAT | O_TRUNC | O_RDWR, S_IRWXU)) == -1)
        my_err("open", __LINE__);
    
    // 设置文件打开方式, 只有 O_APPEND O_NONBLOCK O_ASTNC 三种标志
    if ((ret = fcntl(fd, F_SETFL, O_APPEND)) < 0)
        my_err("fcntl", __LINE__);

    // 获取文件打开方式, 第三个参数 arg 可以忽略
    if ((ret = fcntl(fd, F_GETFL, 0)) < 0)
        my_err("fcntl", __LINE__);
    
    access_mode = ret & O_ACCMODE;
    // O_ACCMODE(3) -> 取得文件打开方式的掩码(取得 ret 最后两位的值)
    // --> http://blog.sina.com.cn/s/blog_6f916d330101603b.html
    // 看fcntl.h文件便可知道，文件的oflag各是什么样的值，有个特殊的
    // O_RDONLY(0),O_WRONLY(1),O_RDWR(2),在二进制中不是各占一位，？？？
    // 所以这三个值的设置和获取用屏蔽字 O_ACCMODE。
    // ---------------------------------------------------------

    // 判断文件打开方式并输出
    if (access_mode == O_RDONLY)            // #define O_RDONLY	     00 ->  0000,0000
        printf("example_64 access mode: read only");
    else if (access_mode == O_WRONLY)       // #define O_WRONLY	     01 ->  0000,0001
        printf("example_64 access mode: write only");
    else if (access_mode == O_RDWR)         // #define O_RDWR		 02 ->  0000,0010
        printf("example_64 access mode: read + write");

    // printf("ret -> %#o\n", ret);                       // o:0102002  -> 1000,0100 0000,0010  (33794)10

    // ret 是 各个参数 | 运算的结果， 若要看是否含有某参数， 只需要让 ret & 该参数 (如果 |， 那么有某种参数的那一位上会设为1，& 之后结果不会为 0))

    if (ret & O_APPEND)         // # define O_APPEND	       02000    -> 0000,0100 0000,0000  (1024)10
        printf(" , append");
    if (ret & O_NONBLOCK)       // # define O_NONBLOCK	       04000    -> 0000,1000 0000,0000  (2048)10
        printf(" , nonblock");
    if (ret & O_SYNC)           // # define O_SYNC	      04010000 -> 0010 0000,0010 0000,0000  (1052672)10
        printf(" , sync");
    
    printf("\n");

    return 0;
}