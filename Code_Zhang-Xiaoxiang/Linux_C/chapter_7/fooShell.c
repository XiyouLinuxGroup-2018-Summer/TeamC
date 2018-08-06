#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <signal.h>
#include <pwd.h>
#include <sys/param.h>
#include <readline/readline.h>      // 使用 readline 处理 tab 补全(默认)
#include <readline/history.h>       // 处理 history


#define CLOSE "\001\033[0m\002"                 // 关闭所有属性
#define BLOD  "\001\033[1m\002"                 // 强调、加粗、高亮
#define BEGIN(x,y) "\001\033["#x";"#y"m\002"    // x: 背景，y: 前景

struct passwd *psd;                     
char hostname[50], cwd[PATH_MAX],temppath[PATH_MAX], flagforuser;
char oldcwd[PATH_MAX] = "";

void InitForPrompt();                   // 打印提示符
int  GetInput(char *);                  // 获取输入, 输入无效返回-1，否则返回0
void ExpInput(char * input, int * argcount, char arglist[100][256]);      // 解析输入
void InitForCmd(int, char [100][256]);  // 根据 &, | , < , > , >> 做相应判断，并调用或 cd/history/DoCmd 执行
void ShowHistory();                     // 内置 history
void ChdirShell(char * argpath[]);      // 内置 cd, 以空指针做为结束标志
void CopyFileC(int, int);               // 拷贝文件内容 
void hanlder(int sig)
{
    fflush(stdout);
    printf("\n");
}

int main(int argc, char * argv[], char ** envp)
{       
    int argcount;
    char arglist[100][256];             // 参数列表
    char * buf = (char *)malloc(256);

    read_history(NULL);                                // 读取历史命令文件

    while(1)
    {
        signal(SIGINT, hanlder);                        // crtl + c 重新执行程序而不是屏蔽
        signal(SIGTTIN, SIG_IGN);
        signal(SIGTSTP, SIG_IGN);
        signal(SIGHUP, SIG_IGN);
        memset(buf, '\0', sizeof(buf)); 

        InitForPrompt();                                // 为打印提示符获取相关信息
        if (GetInput(buf) == -1)                        // 如果输入错误，重新开始
            continue;
        if (strcmp(buf, "exit\n") == 0 || strcmp(buf, "logout\n") == 0)     // 退出程序
            break;
        memset(arglist, '\0', sizeof(arglist));         // 清空参数列表

        argcount = 0;
        ExpInput(buf, &argcount, arglist);              // 解析参数列表
        InitForCmd(argcount, arglist);
    }
    
    if (buf != NULL)
    {
        free(buf);
        buf = NULL;
    }
    exit(0);
}

void InitForPrompt()          
{
    strcpy(temppath, "/home/");
    pid_t id = getuid();    
    psd = getpwuid(id);
    gethostname(hostname, 50 - 1);       // 获取主机名
    hostname[49] = '\0';
    getcwd(cwd, PATH_MAX - 1);          // 获取当前工作目录
    strcat(temppath, psd->pw_name);
    int len = strlen(temppath);
    if (strncmp(cwd, temppath, len) == 0)
    {
        strcpy(temppath, "~");
        strcat(temppath, cwd + len);
    }
    else
        strcpy(temppath, cwd);
}

int GetInput(char * buf)
{    
    int len = 0;
    fflush(stdout);
    if (getuid())
        flagforuser = '$';
    else
        flagforuser = '#';
    char temp[300];                     // 不把字符串当成参数传入 readline ， 上下切换命令会导致格式混乱
    sprintf(temp, BEGIN(49,33) "%s@%s>" BEGIN(49,34) " %s" BLOD CLOSE "%c ", hostname, psd->pw_name, temppath, flagforuser);
    char * istream = readline(temp);
    if (strcmp(istream, "") == 0)
        return -1;
    char * copyis = istream;
    add_history(istream);               // 加入 history
    write_history(NULL);                // 将命令历史写入文件
    while(*(copyis++) != '\0')          // 计算输入命令长度
        len++;        
    if (++len >= 255)
    {
        printf("Command is too long.\n");
        free(istream);
        return -1;
    }
    strcpy(buf, istream);
    buf[len - 1] = '\n';                // 以换行符结束
    buf[len] = '\0';

    free(istream);                      // 释放分配的内存                
    return 0;
}

void ChdirShell(char * argpath[])       // 只允许一个路径
{
    if (argpath[1] != NULL && argpath[2] != NULL)        // 只支持一个或零个参数
    {
        printf("too many arguements\n");
        return;
    }
    if (argpath[1] != NULL && strcmp(argpath[1], "~") == 0)
        argpath[1] = NULL;
    if (argpath[1] == NULL)
    { 
        char tempforhome[PATH_MAX] = "/home/";
        strcat(tempforhome, psd->pw_name);
        if(chdir(tempforhome) == -1)
            perror("chdir");
        strcpy(oldcwd, cwd);
    }
    else
    {
        if (strcmp(argpath[1], "-") == 0)
        {
            if (oldcwd[0] == '\n')
                return;
            // printf("oldcwd:%s\n", oldcwd);
            if (chdir(oldcwd) == -1)
                perror("chdir");            
        }
        else if (chdir(argpath[1]) == -1)
            perror("chdir");
        strcpy(oldcwd, cwd);
    }
}

void ShowHistory()
{
    int i = 0;
    HIST_ENTRY ** his;
    his = history_list();
    while(his[i] != NULL)
    {
        printf("%s\n", his[i]->line);
        i++;
    }
}

void ExpInput(char * input, int * argcount, char arglist[100][256])
{
    char * p = input, * q = input;
    int number;

    while(*p != '\n')
    {
        while (*p == ' ')       // 越过空格
            p++; 
        if (*p == '\n')
            break;
        number = 0;
        q = p;                  // for strncpy
        while (*p != '\n' && *p != ' ')
        {
            number++;
            p++;       
        }
        strncpy(arglist[*argcount], q, number);
        arglist[(*argcount)++][number] = '\0';
    }
}

void InitForCmd(int argcount, char arglist[100][256])
{
    int pid;                    // 子进程pid
    int status;                 // waitpid(....)
    int flag = 0;               // pipe 数量, 以管道为分界线执行多个命令和重定向
    int isBackground = 0;       // 检查是否有后台运行标识符
    char * arg[argcount + 50];  // 真正会执行的参数列表
    memset(arg, '\0', sizeof(arg));
    char * tempfile = "/tmp/fooshelltemp";
    char * Minput = "/tmp/pxShellPipeVirtualInput";
    char * Moutput = "/tmp/pxShellPipeVirtualoutput";
    int fin, fout;

    for (int i = 0; i < argcount; i++)          // 方便对命令分析和执行命令
        arg[i] = (char *)arglist[i];
    arg[argcount] = NULL;                       // 命令以　NULL　结束

    for (int i = 0; i < argcount; i++)
    {
        if (strcmp(arg[i], "|") == 0)           // 计算重定向符号的数量
        {
            flag++;
            arg[i] = NULL;
            if (arg[i + 1] == NULL)             // 重定向符号后面必须有命令
            {
                fprintf(stderr, "wrong cmd\n");
                return;
            }
        }
        else if (strcmp(arg[i], "&") == 0)      // 如果后台运行标识符不在命令末尾，报错
        {
            if (i == argcount - 1)
            {
                isBackground = 1;
                arg[argcount - 1] = NULL;       // 命令以　NULL 结尾
            }
            else
            {
                fprintf(stderr, "wrong cmd\n");
                return;
            }
        }
    }
    // 动态分配存储重定向文件名的数组, 每种重定向最多支持 10 个文件, 以及标示输出重定向是 '>' 还是 '>>'
    char* (* file_for_input)[10]  = (char* (*) [10])calloc(sizeof(char*), (flag + 1) * 10);
    char* (* file_for_output)[10] = (char* (*) [10])calloc(sizeof(char*), (flag + 1) * 10);
    int (* redirect_flag)[10] = (int (*) [10])calloc(sizeof(int), (flag + 1) * 10); // 1: > 2: >>
    // 重定向参数处理
    for (int i = 0, j = 0; i <= flag; i++)             // 有 flag + 1 个命令
    {   
        int Icount = 0, Ocount = 0;
        int n = j;
        
        while (arg[j] != NULL)
        {   
            if (strcmp(arg[j], "<") == 0)
            {
                if (arg[j + 1] == NULL)
                {
                    fprintf(stderr, "ERROR: command %c filename\n",'<');
                    return;
                }
                file_for_input[i][Icount] = arg[j + 1];      // 记录文件名
                arg[j] = arg[j + 1] = NULL;                  // 标记为NULL
                Icount++;
                j += 1;
            }
            else if (strcmp(arg[j], ">>") == 0 || strcmp(arg[j], ">") == 0)
            {   
                if (arg[j + 1] == NULL)
                {
                    fprintf(stderr, "ERROR: command %s filename\n", arg[j]);
                    return;
                }
                redirect_flag[i][Ocount] = strlen(arg[j]);
                file_for_output[i][Ocount] = arg[j + 1];
                arg[j] = arg[j + 1] = NULL;
                Ocount++;
                j += 1;
            }
            j++;
        }

        if (i != flag)
            file_for_output[i][Ocount++] = tempfile;
        j++;
        // 调整参数列表
        for (int kk = n; kk < j; kk++)
        {
            if (arg[kk] == NULL)
            {
                int m = kk;
                while (arg[m] == NULL && m < j)
                    m++;
                if (m >= j)
                    break;
                arg[kk] = arg[m];
                arg[m] = NULL;
            }
        }
    }
    int flag_output;
    for (int i = 0, j = 0; i <= flag; i++)
    {
        // 输入流整合
        fin = open(Minput, O_CREAT | O_TRUNC | O_RDONLY, 0644);
        close(fin);
        fin = open(Minput, O_RDWR | O_APPEND);                      // 打开模拟的输入缓存
        for (int t = 0; file_for_input[i][t] != NULL; t++)
        {
            int fd_new = open(file_for_input[i][t], O_RDONLY);
            if (fd_new < 0)
                perror("open");
            // printf("%d\t[file1] %s\n", __LINE__, Minput);
            // printf("%d\t[file2] %s\n", __LINE__, file_for_input[i][t]);
            CopyFileC(fin, fd_new);
            close(fd_new);
        }
        if (i == 0)
            unlink(tempfile);
        if (flag)
        {
            int fd_new = open(tempfile, O_RDONLY | O_CREAT, 0644);
            if (fd_new < 0)
                perror("open");
            CopyFileC(fin, fd_new);
            close(fd_new);
        }
        close(fin);
        pid = fork();
        int success = 0;
        switch(pid)
        {
            case -1:
                perror("fork failed.");
                close(fin);
                return;
            case 0:
                // 重定向处理
                flag_output = 0;
                if (flag || file_for_input[i][0])     // 如果有管道或输入重定向
                {
                    fin = open(Minput, O_RDONLY);
                    if (dup2(fin, STDIN_FILENO) == -1)
                        perror("dup2 > fin");
                }
                for (int t = 0; file_for_output[i][t] != NULL; t++)     // 如果有管道或输出重定向
                {
                    flag_output = 1;
                    pid_t pidcc = fork();
                    if (pidcc == 0)
                    {
                        if (redirect_flag[i][t] == 2)
                            fout = open(file_for_output[i][t], O_RDWR | O_APPEND| O_CREAT , 0644);
                        else
                            fout = open(file_for_output[i][t], O_RDWR | O_TRUNC| O_CREAT , 0644);
                        dup2(fout, STDOUT_FILENO);

                        if (strcmp(arg[j], "cd") != 0 && strcmp(arg[j], "history") != 0)
                        {
                            if ((success = execvp(arg[j], arg + j)) == -1)
                                perror("EXEC>");
                        }
                        if (strcmp(arg[j], "history") != 0)
                            ShowHistory();
                        close(fout);
                        exit(0);
                    }
                    if (i != flag || isBackground)
                        wait(&status);
                }
                if (!flag_output)
                {
                    if (strcmp(arg[j], "cd") != 0 && strcmp(arg[j], "history") != 0)
                    {
                        if (execvp(arg[j], arg + j) == -1)
                            perror("EXEC>");
                    }
                    if (strcmp(arg[j], "history") == 0)
                        ShowHistory();
                }
                exit(0);
            default:
                if (strcmp(arg[j], "cd") == 0)
                    ChdirShell(arg + j);                                    
                break;
        }
        // 获取到下一个有效参数
        while(arg[j] != NULL)
            j++;
        while(arg[j] == NULL)
            j++;
        if (i == flag && isBackground)      // 后台执行、子进程直接返回
        {
            signal(SIGCHLD, SIG_IGN);       // 防止子进程成为僵死进程
            printf("[process id %d]\n", pid);
            break;
        }
        // else if (waitpid(pid, &status, 0) == -1)                // 父进程等待子进程结束
            // perror("wait for child process error\n");
        else 
            waitpid(pid, &status, 0);
    }
    // 释放空间
    free(file_for_input);
    free(file_for_output);
    free(redirect_flag);
}

void CopyFileC(int fd_orign, int fd_new)
{
    errno = 0;
    char buf[1024];
    memset(buf, '\0', sizeof(buf));
    lseek(fd_new, 0, SEEK_SET);
    int len;
    while((len = read(fd_new, buf, 1024)) != 0)
    {
        write(fd_orign, buf, len);
    }
    if (errno)
        perror("Copy Error");
}
