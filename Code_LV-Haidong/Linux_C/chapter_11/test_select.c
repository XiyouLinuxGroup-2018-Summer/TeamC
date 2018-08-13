//示例select()的使用
//如果再循环中调用select函数，则必须让time初始化也在循环内部
//不然很可能造成死循环
#include<stdio.h>
#include<sys/time.h>
#include<sys/types.h>
#include<unistd.h>
#include<time.h>
#include<stdlib.h>
void display_time(const char * string)
{
      int seconds;

      seconds = time((time_t *)NULL);
      printf("%s, %d\n",string,seconds);
}

int main()
{
    fd_set readfds;//文件描述符集合
    struct timeval timeout;
    int ret;
    //监事文件描述符0有没有数据输入,文件描述符0为标准输入，即键盘输入
    FD_ZERO(&readfds);//开始使用一个描述符即和前一般要将其清空
    FD_SET(0,&readfds);//添加文件描述符

    //设置阻塞时间
    timeout.tv_sec =5;
    timeout.tv_usec = 0;

    while(1)
    {
        display_time("before select");
        ret = select(1,&readfds,NULL,NULL,&timeout);
        display_time("after select");
        switch(ret)
        {
            case 0: printf("NO data in five seconds. \n");
                    exit(0);
                    break;
            case -1:
                    perror("select");
                    exit(1);
                    break;
            default:
                   getchar(); //读入数据，不然标准输入将一直显示读就绪
                   printf("Data is available now.\n");
                   break;
        }
    }
}
