#include <stdio.h>
#include <time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

struct stat
{
    dev_t  st_dev;        //文件的设备编号
    ino_t  st_ino;        //文件i的节点编号
    mode_t  st_mode;      //文件类型 存取权限
    nlink_t  st_nlink;    //连接到该文件的硬链接数目
    uid_t  st_uid;        //文件所有者的用户ID
    gid_t  st_gid;        //文件所有者的组ID
    dev_t  st_rdev;       //若此文件为设备文件 则其为设备编号
    off_t  st_size;       //文件大小
    blksize_t st_blksize; //文件系统的I/O缓冲区大小
    blkcnt_t  st_blocks;  //占用文件块的个数
    time_t  st_atime;     //文件最近一次被访问的时间
    time_t  st_mtime;     //文件最近一次被修改的时间
    time_t  st_ctime;     //文件最近一次被更改的时间
};

int main(int argc, char * argv[])
{
    struct stat buf;
    //检查参数个数
    if(argc != 2)
    {
        printf("Usage: my_stat <filename>\n");
	exit(0);
    }

    //获取文件属性
    if(stat(argv[1], &buf) == -1)
    {
        perror("stat:");
	exit(1);
    }

    //打印出文件属性
    printf("device is: %d\n", buf.st_dev);
    printf("inode is: %d\n", buf.st_ino);
    printf("mode is: %c\n", buf.st_mode);
    printf("number of hard links is: %d\n", buf.st_nlink);
    printf("user ID of owner is: %d\n", buf.st_uid);
    printf("group ID of owner is: %d\n", buf.st_gid);
    printf("device type (if inode device) is: %d\n", buf.st_rdev);
    printf("total size, in bytes is: %d\n", buf.st_size);
    printf("blocksize for filesystem I/O is: %d\n", buf.st_blksize);
    printf("number if blocks allocated is: %d\n", buf.st_blocks);
    printf("time if last access is: %s", ctime(&buf.st_atime));
    printf("timr of last modification is: %s", ctime(&buf.st_mtime));
    printf("time of last change is: %s", ctime(&buf.st_ctime));

    return 0;
}
