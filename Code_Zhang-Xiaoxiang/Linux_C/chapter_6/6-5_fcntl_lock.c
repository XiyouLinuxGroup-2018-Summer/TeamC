/**
* 6-5_fcntl_lock.c
* @author: PhoenixXC
* @email: xuancbm@xiyoulinux.org
* @description: 文件锁的应用实例
* @created: Tue Jul 24 2018 16:49:15 GMT+0800 (CST)
* @last-modified: Tue Jul 24 2018 16:49:15 GMT+0800 (CST)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

// 锁的设置或释放
int lock_set(int fd, struct flock * lock)
{
    if (fcntl(fd, F_SETLK, lock) == 0)
    {
        if (lock->l_type == F_RDLCK)
            printf("set read lock, pid: %d\n", getpid());   // #include <unistd.h> getpid — get the process ID
        else if (lock->l_type == F_WRLCK)
            printf("set write lock,  pid: %d\n", getpid());
        else if (lock->l_type == F_UNLCK)
            printf("release lock, pid: %d\n", getpid());
    }
    else
    {
        perror("lock operation fail\n");
        return -1;
    }

    return 0;
}

// 测试锁，只有当测试发现参数 lock 指定的锁能被设置时， 返回 0
int lock_test(int fd, struct flock * lock)
{
    if (fcntl(fd, F_GETLK, lock) == 0)      
    {
        if (lock->l_type == F_UNLCK)            // 可以被设置为 *lock 这样的锁
        {
            printf("lock can be set in fd\n");
            return 0;
        }
        else                                    // 如果锁冲突
        {
            if (lock->l_type == F_RDLCK)
                printf("can't set lock, read lock has been set by: %d\n", lock->l_pid); // 打印出该锁的进程
            else if (lock->l_type == F_WRLCK)
                printf("can't set lock, write lock has been set by: %d\n", lock->l_pid);

            return -1;
        }
    }
    else                                        // 执行失败
    {
        perror("get incompatible locks fail");
        return -1;
    }
}

int main(void)
{
    int     fd;
    int     ret;
    struct  flock lock;
    char    read_buf[32];

    // 打开或创建文件
    if ((fd = open("example_65", O_CREAT | O_TRUNC | O_RDWR, S_IRWXU)) == -1)
        my_err("open", __LINE__);

    // 写入数据
    if (write(fd, "test lock", 10) != 10)
        my_err("write", __LINE__);

    // 初始化 lock 结构
    memset(&lock, 0, sizeof(struct flock));         // memset 可以对结构进行操作， memset(&strcut name, 0, sizoef(struct name))
    lock.l_start = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len = 0;

    // 设置读锁
    lock.l_type = F_RDLCK;
    if (lock_test(fd, &lock) == 0)
    {
        lock.l_type = F_RDLCK;      // 恢复锁的设置
        lock_set(fd, &lock);
    }

    // 读数据
    lseek(fd, 0, SEEK_SET);
    if ((ret = read(fd, read_buf, 10)) < 0)
        my_err("read", __LINE__);
    
    read_buf[ret] = '\0';
    printf("%s\n", read_buf);

    // waitting
    getchar();

    // 锁的不兼容规则针对多个进程之间
    // 设置写锁
    lock.l_type = F_WRLCK;
    if (lock_test(fd, &lock) == 0)
    {
        lock.l_type = F_WRLCK;
        lock_set(fd, &lock);
    }

    // 释放
    lock.l_type = F_UNLCK;
    lock_set(fd, &lock);
    close(fd);

    return 0;
}