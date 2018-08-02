/**
* 7-12_myshell.c
* > author: PhoenixXC
* > email: xuancbm@xiyoulinux.org
* > description: the code from book
* > created: Wed Aug 01 2018 10:14:50 GMT+0800 (CST)
* > last-modified: Wed Aug 01 2018 10:14:50 GMT+0800 (CST)
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <dirent.h>

#define normal              0   // 一般命令
#define out_redirect        1   // 输出重定向
#define in_redirect         2   // 输入重定向
#define have_pipe           3   // 管道

void print_prompt();                            // 打印提示符
void get_input(char *);                         // 得到输出的命令
void explain_input(char *, int *, char a[][]);  // 解析执行的命令
void do_cmd(int, char a[][]);                   // 执行命令
int find_command(char *);                       // 查找命令中的可执行程序

int main(int argc, char * argv[])
{
    int i;
    int argcount = 0;
    char arglist[100][256];
    char ** arg = NULL;
    char * buf = NULL;

    buf = (char *)malloc(256);  // 存储输入信息
    if (buf == NULL)
    {
        perror("malloc failed");
        exit(-1);
    }

    while (1)
    {
        // 清空buf指向的空间，malloc不会为新分配的空间清零
        memset(buf, 0, 256);
        print_prompt();                // 打印提示符
        get_input(buf);
        // cmd 为 exit 或 logout 退出程序
        if (strcmp(buf, "exit") == 0 || strcmp(buf, "logout") == 0)
            break;
        // 
        for (i = 0; i < 100; i++)
            arglist[i][0] = '\0';
        argcount = 0;
        explain_input(buf, &argcount, arglist);
        do_cmd(argcount, arglist);
    }

    // 如果分配的内存没有被释放，释放内存
    if (buf != NULL)    
    {
        free(buf);
        buf = NULL;
    }

    exit(0);
}

void print_prompt()
{
    printf("myShell$$ ");
}

void get_input(char * buf)
{
    int len = 0;
    int ch;

    // 处理用户输入
    ch = getchar();
    while (len < 256 && ch != '\n')
    {
        buf[len++] = ch;
        ch = getchar();
    }

    // 如果输入太长，报错退出
    if (len == 256)
    {
        printf("command is too long\n");
        exit(-1);
    }

    buf[len] = '\n';
    len++;
    buf[len] = '\0';
}

// 解析执行的命令，buf：输入数据   argcount : 参数数目      arglist: 参数列表
void explain_input(char * buf, int * argcount, char arglist[100][256])
{
    char * p = buf;
    char * q = buf;
    int number = 0;

    while (1)
    {
        if (p[0] == '\n')
            break;
        if (p[0] == ' ')
            p++;
        else
        {
            q = p;
            number = 0;
            while ((q[0] != ' ') && (q[0] != '\n'))
            {
                number++;   // 单词长度
                q++;
            }
            strncpy(arglist[*argcount], p, number + 1);
            arglist[*argcount][number] = '\0';      // 以空字符结尾
            *argcount = *argcount + 1;
            p = q;
        }
    }
}

int find_command(char * command)
{
    DIR * dp;
    struct dirent * dirp;
    char * path[] = {"./", "/bin", "/usr/bin", NULL};   // NULL 寻找路径结束的标记

    // 使当前目录下的程序可以运行
    // 如果是当前目录下的程序，除去前面的 "./"
    if (strncmp(command, "./", 2) == 0)
        command = command + 2;

    int i = 0;
    while (path[i] != NULL)
    {
        if ((dp = opendir(path[i])) == NULL)
            printf("can not open %s\n", path[i]);
        // 开始寻找
        while ((dirp = readdir(dp)) != NULL)
        {
            // 如果查找到了，return 1
            if (strcmp(dirp->d_name, command) == 0)
            {
                closedir(dp);
                return 1;
            }
        }
        closedir(dp);
        i++;
    }
    return 0;   // 查找失败返回0
}

// argcount 参数数目        arglist 参数列表
void do_cmd(int argcount, char arglist[100][256])
{
    int flag = 0;
    int how = 0;            // 指示命令是否含有 > < |
    int background = 0;     // 标示是否有后台运行标识符
    int status;             //
    int i;
    int fd;

    char * arg[argcount + 1];           // 动态？？ 参数的拷贝
    char * argnext[argcount + 1];
    char * file;

    pid_t pid;

    // 取出命令
    for (i = 0; i < argcount; i++)
    {
        arg[i] = (char *)arglist[i];
    }
    arg[argcount] = NULL;              // 重点

    // 查看命令行是否有后台运行符
    for (i = 0; i < argcount; i++)
    {
        if (strncmp(arg[i], "&, 1") == 0)
        {
            if (i == argcount - 1)     // 如果后台运行标志符在最后
            {
                background = 1;
                arg[argcount - 1] = NULL;
                break;
            }
            else
            {
                printf("wrong command\n");
                return;
            }
        }
    }
    // 输出重定向
    for (i = 0; arg[i] != NULL; i++)
    {
        if (strcmp(arg[i], ">") == 0)
        {
            flag++;
            how = out_redirect;
            if (arg[i + 1] == NULL)     // 如果重定向未指定文件，命令格式出错
                flag++;
        }
        if (strcmp(arg[i], "<") == 0)
        {
            flag++;
            how = in_redirect;
            if (arg[i + 1] == NULL)
                flag++;
        }
        if (strcmp(arg[i], '|') == 0)
        {
            flag++;
            how = have_pipe;
            if (arg[i + 1] == NULL)
                flag++;
            if (i == 0)
                flag++;
        }
    }
    // 命令格式错误就退出
    if (flag > 1)
    {
        printf("wrong command.\n");
        return;
    }
    // 输出重定向
    if (how == out_redirect)
    {
        for (i = 0; arg[i] != NULL; i++)
        {
            if (strcmp (arg[i], "<") == 0)
            {
                file = arg[i + 1];
                // 确定文件名
                arg[i] = NULL;  
                // 将之前的命令和文件名分隔开
            }
        }
    }
    // 输入重定向
    if (how == in_redirect)
    {
        for (i = 0; arg[i] != NULL; i++)
        {
            if (strcmp (arg[i], "<") == 0)
            {
                file = arg[i + 1];
                // 确定文件名
                arg[i] = NULL;  
                // 将之前的命令和文件名分隔开
            }
        }
    }

    // 命令中含有管道
    if (how == have_pipe)
    {
        for (i = 0; arg[i] != NULL; i++)
        {
            if (strcmp(arg[i], "|") == 0)
            {
                arg[i] = NULL;
                int j;
                // 依次把管道后面的部分存入argnext中， 管道后面是可执行的shell命令
                for (j = i + 1; arg[j] != NULL; j++)
                {
                    argnext[j - i - 1] = arg[j];
                }
                argnext[j - i - 1] = arg[i];    // 结尾设为空指针
                break;
            }
        }
    }

    // 创建子进程1号
    if ((pid = fork()) < 0)
    {
        printf("fork error\n");
        return;
    }

    switch(how)
    {
        // 只让子进程执行
        case 0:
            // arg[0] 可执行程序名， arg ： 参数列表，以NULL结尾
            if (pid == 0)
            {
                if (!(find_command(arg[0])))
                {
                    printf("%s : command not found\n", arg[0]);
                    exit(0);
                }
                execvp(arg[0], arg);
                exit(0);
            }
            break;
        case 1:
            // 输出重定向
            if (pid == 0)
            {
                if (!find_command(arg[0]))
                {
                    printf("%s : command not found\n", arg[0]);
                    exit(0);
                }
                fd = open(file, O_RDWR | O_CREAT | O_TRUNC, 0644);
                dup2(fd, 1);    // 重定向
                execvp(arg[0], arg);
                exit(0);
            }
            break;
        case 3:
            if(pid )
    }
}

