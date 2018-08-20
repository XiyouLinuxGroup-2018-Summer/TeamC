#include "base.h"

// fun: 无回显getchar()
int getch(void)
{
    struct termios tm, tm_old;
    int fd = 0, ch;

    if (tcgetattr(fd, &tm) < 0)                 //保存现在的终端设置
        return -1;
    tm_old = tm;
    cfmakeraw(&tm);                             //更改终端设置为原始模式，该模式下所有的输入数据以字节为单位被处理
    if (tcsetattr(fd, TCSANOW, &tm) < 0)        //设置上更改之后的设置
        return -1;
    ch = getchar();
    if (tcsetattr(fd, TCSANOW, &tm_old) < 0)    //更改设置为最初的样子
        return -1;
    return ch;
}

// fun: 获取当前时间
void get_time(char * string)
{
    time_t now;                 //实例化time_t结构
    struct tm * timenow;        //实例化tm结构指针
    time(&now);
    //time函数读取现在的时间(国际标准时间非北京时间)，然后传值给now
    timenow = localtime(&now);
    //localtime函数把从time取得的时间now换算成你电脑中的时间(就是你设置的地区)
    // printf("Local time is %s\n",asctime(timenow));
    strcpy(string, asctime(timenow));
}

// fun: 行输入
char * s_gets(char * st, int n, FILE *fp)
{
    char * ret_val, * find;
    ret_val = fgets(st, n, fp);
    if (ret_val)   
    {
        find = strchr(st, '\n');
        if (find)            
            *find = '\0';
        else            
            while (getchar() != '\n')
                ;
    }
    return ret_val;
}

// fun: 无回显输入单行字符串
char* s_getchs(char * st, int n)
{
    int ch, count = 0;
    memset(st, 0, n);
    while (count < n)
    {
        ch = getch();
        if (ch == 13)
            break;
        else if (ch != 8 && ch != 127)
            st[count++] = ch;
        else 
            count = count - 1 == -1 ? count : count - 1;
    }
    st[count] = '\0';
    return st;
}
