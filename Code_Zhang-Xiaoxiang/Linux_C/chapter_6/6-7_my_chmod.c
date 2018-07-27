/**
* 6-7_my_chmod.c
* @author: PhoenixXC
* @email: xuancbm@xiyoulinux.org
* @description: 获取文件属性
* @created: Tue Jul 24 2018 20:04:53 GMT+0800 (CST)
* @last-modified: Tue Jul 24 2018 20:04:53 GMT+0800 (CST)
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

int main(int argc, char * argv[])
{
    struct stat buf;

    // 检查参数个数
    if (argc != 2)
    {
        printf("Usage: my_stat <filename>\n");
        exit(1);
    }
    // 获取文件属性
    if (stat(argv[1], &buf) == -1)
    {
        perror("stat");
        exit(1);
    }

    // 打印出文件属性
    printf("device is: %d\n", buf.st_dev);
    printf("inode is: %d\n", buf.st_ino);
    printf("mode is: %d\n", buf.st_mode);
    printf("number of hard links is: %d\n", buf.st_nlink);
    printf("user ID of owner is: %d\n", buf.st_uid);
    printf("group ID of owner is: %d\n", buf.st_gid);
    printf("device type (if inode device) is: %d\n", buf.st_rdev);

    printf("total size, in bytes is: %d\n", buf.st_size);
    printf("blocksize for filesystem I/O is: %d\n", buf.st_blksize);
    printf("number of blocks allocated is: %d\n", buf.st_blocks);

    printf("time of last access is: %d\n", buf.st_atime);
    printf("time of last modification is: %d\n", buf.st_mtime);
    printf("time of last change is: %d\n", buf.st_ctime);
    // 系统记录的时间是一个整数，这个整数表示自1970年1月1日00:00:00（国际标准时间）
    // 以来到时间记录所要表达的时间所经过的秒数   --------------------- 日历时间

    struct tm * easyread = localtime(&buf.st_mtime);    // 转换为当地时区的时间
    // 以能看懂的方式打印 mtime
    // https://blog.csdn.net/q1007729991/article/details/53405716
    printf("%4d 年 ", easyread->tm_year + 1900);
    printf("%2d 月 ", easyread->tm_mon + 1);
    printf("%2d 日 ", easyread->tm_mday);
    printf("第 %d 周 ", easyread->tm_wday + 1);
    printf("%2d分 ", easyread->tm_hour);
    printf("%2d秒 \n", easyread->tm_min);
 
    return 0;
}