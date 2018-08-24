/**
* Untitled-1
* > author: PhoenixXC
* > email: xuancbm@xiyoulinux.org
* > description: use select()
* > created: Mon Aug 13 2018 15:43:28 GMT+0800 (CST)
* > last-modified: Mon Aug 13 2018 15:43:28 GMT+0800 (CST)
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>

void display_time(const char * string)
{
    int seconds;
    seconds = time((time_t *)NULL);
    printf("%s, %d\n", string, seconds);
}

int main(void)
{
    fd_set readfds;
    struct timeval timeout;
    int    ret;

    // 监视文件描述符0是否有数据输入，文件描述符0表示标准输入
    FD_ZERO(&readfds);
    FD_SET(0, &readfds);

    // 设置阻塞时间为10秒
    // timeout.tv_sec = 10;
    // timeout.tv_usec = 0;

    while (1)
    {
        // -------------------
        timeout.tv_sec = 10;
        timeout.tv_usec = 0;
        // -------------------
        display_time("Before select");
        ret = select(1, &readfds, NULL, NULL, &timeout);
        display_time("after select");

        switch(ret)
        {
            case 0:
                printf("No data in ten seconds.\n");
                exit(0);
                break;
            case -1:
                printf("select");
                exit(1);
                break;
            default:
                getchar();
                printf("Data is avaliable now.\n");                
        }
    }

    return 0;
}
