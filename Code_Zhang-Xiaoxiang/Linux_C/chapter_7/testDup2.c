// #include <stdio.h>
// #include <unistd.h>
// #include <fcntl.h>
// #include <sys/stat.h>
// #include <sys/types.h>

// int main(void)
// {
//     printf("test dup2\n");

//     int fd0 = open("/tmp/test.c", O_RDWR|O_CREAT|O_TRUNC, S_IRWXU);
//     if (fd0 < 0)
//     {
//         printf("Open failed\n");
//         return 0;
//     }

//     int fd1 = dup2(fd0, 1);
//     if (fd1 < 0)
//     {
//         printf("Dup2 fialed\n");
//         return 0;
//     }

//     if (write(1, "hello world", 11) < 0)
//         printf("Error");

//     close(fd0);
// }
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main()
{

    int fd =  open("/tmp/test.c", O_RDWR | O_CREAT, 0666);
    if (fd < 0)
    {
        fprintf(stderr, "open failure\n");
        exit(1);
    }
    printf("-> stdout:\n");
    printf("fd = %d\n", fd);
    // 开始重定向

    close(1);
    int new_fd = dup(fd);

    printf("-> file :\n");
    printf("new_fd = %d\n", new_fd);
    fflush(stdout);
    return 0;
}
