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
    time_t now;                 
    struct tm * timenow;        
    time(&now);                 
    timenow = localtime(&now);
    char temptime[55];
    sprintf(temptime, "%d-%02d-%02d %d:%d:%d", timenow->tm_year + 1900, timenow->tm_mon + 1, timenow->tm_mday, timenow->tm_hour, timenow->tm_min, timenow->tm_sec);
    if (string == NULL)
        printf("%s\n", temptime);
    else
        strcpy(string, temptime);
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

void my_err(const char * file, const char * err_string, int line, int flag)
{
    fprintf(stderr, "File: %s\n", file);
    fprintf(stderr, "line: %d ", line);
    perror(err_string);
    if (!flag)
        exit(1);
    // else
        // pthread_exit(0);
}
