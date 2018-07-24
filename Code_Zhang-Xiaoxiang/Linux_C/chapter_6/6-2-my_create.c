/**
* 6-2-my_create.c
* @author: PhoenixXC
* @email: xuancbm@xiyoulinux.org
* @description: 
* @created: Tue Jul 24 2018 10:15:26 GMT+0800 (CST)
* @last-modified: Tue Jul 24 2018 10:15:26 GMT+0800 (CST)
*/

#include <stdio.h>          // perror()
#include <string.h>         // strerror()
#include <stdlib.h>         // exit()
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>          // errno
#include <unistd.h>

// > en.wikipedia.org
// The following lists typical operations on file descriptors on modern Unix-like systems. 
// Most of these functions are declared in the <unistd.h> header, but some are in the <fcntl.h> header instead.
// 现代类Unix系统上对文件描述符的典型操作的函数大部分在头文件 <unistd.h> ， 也有一些在 <fcntl.h> 中
// fcntl.h is the header in the C POSIX library for the C programming language that contains constructs that 
// refer to file control, e.g. opening a file, retrieving and changing the permissions of file, locking a file for edit, etc.

int main(void)
{
    int fd;

    // if ((fd = creat("6.1-my_chmod.c", S_IRWXU)) == -1)
    // # define S_IRWXU	(__S_IREAD|__S_IWRITE|__S_IEXEC)  包含在头文件：<fcntl.h>
    // #define	__S_IREAD	0400	/* Read by owner.  */
    // #define	__S_IWRITE	0200	/* Write by owner.  */
    // #define	__S_IEXEC	0100	/* Execute by owner.  */
    if ((fd = open("6.1-my_chmod.c",  O_CREAT | O_EXCL, S_IRUSR | S_IWUSR)) == -1)      
    {
        // perror("open");
        printf("open:%s  with errno:%d\n", strerror(errno), errno);
        exit(1);
    }
    else
        printf("create file success\n");
    
    close(fd);
    return 0;
}