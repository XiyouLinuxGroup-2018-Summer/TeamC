#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <strings.h>
 
int main()
{
    int sfd = dup(STDOUT_FILENO), sfd2 = dup(STDIN_FILENO);
    int testfd, testfd2, testfd3;
    int temp, temp2, temp3;
    testfd = open("./temp", O_CREAT | O_RDWR | O_TRUNC, S_IRWXU);
    testfd2 = open("./temp2", O_CREAT | O_RDWR | O_TRUNC, S_IRWXU);
    testfd3 = open("./temp3", O_CREAT | O_RDWR | O_TRUNC, S_IRWXU);
    // for (int i = 0; i < 3; i++)
    // {
    //     printf("Hello world.\n");
    //     test

    //     // 恢复
    //     dup2(sfd,STDOUT_FILENO);
    //     dup2(sfd2, STDIN_FILENO);
    // }
    
    dup2(testfd, 1);        // 输出重定向
    printf("Hello world.\n");
    dup2(sfd,STDOUT_FILENO); // 恢复
    dup2(testfd, 0);
    dup2(testfd2, 1);
    

    close(testfd);
    close(testfd2);
    close(testfd3);
}
