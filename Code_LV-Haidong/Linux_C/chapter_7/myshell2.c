#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <signal.h>
#include <ctype.h>
#include <string.h>
#include <pwd.h>
#include <wait.h>
#include <readline/history.h>
#include <readline/readline.h>
#define MAXARGS 20
#define ARGLEN 60
int i;
struct parameter
{
    int normal;        //一般命令
    int out_redirect;  //输出重定向  >
    int in_redirect;   //输入重定向   <
    int have_pipe;     //命令中由管道符号
    int backgroud;     //标识命令有没有重定向
    int out_redirects; // >>
};
char oldpwd[300][300];
int o;
struct parameter param;
//cd内置命令
void shell_cd(char *path[]);
void find_command(char *path[]);
void get_ifnput(char *buf)
{
    printf("---------------------------------------------------\n");
    struct passwd *name; ///头文件在pwd.h之中，需要添加
    name = getpwuid(getuid());
    char pwd[100] = {0};
    getcwd(pwd, sizeof(pwd) - 1); //保存绝对路径
    int len = strlen(pwd);
    char *p = pwd + len;
    char temp[MAXARGS * ARGLEN];
    int pathlen;
    char *isstarm;
    while (*p != '/' && len--) //把当前目录从后往前遍历
    {
        p--;
    }
    p++;
    sprintf(temp, "[%s @myshell %s] :", name->pw_name, p);
    isstarm = readline(temp);
    add_history(isstarm);
    write_history(NULL);
    pathlen = strlen(temp);
    if (pathlen == MAXARGS * ARGLEN)
    {
        perror("too long");
        exit(-1);
    }
    strcpy(buf, isstarm);
   // buf[pathlen] = '\n';
    buf[pathlen++] = '\0';
    free(isstarm);
}
//解析命令行参数
void explain(char *buf, char *list[256])
{
    i = 0;
    char *p = buf;
    while (1)
    {

        if(strcmp(buf,"cd")==0)
        {
            strcat(buf," ~");
        }
        if (p[0] == '\n')
            break;
        if (p[0] == ' ')
            p++;
        else
        {

            list[i] = strtok(buf, " "); //将他们的空格分开i
            i++;
            while ((list[i] = strtok(NULL, " ")) != NULL && strcmp(list[i], "\n") != 0)
            { //在这里需要注意strtok的返回值为NULL
                i++;
            }
        }
        if (list[i] == NULL)
        {
            break;
        }
    }
}
void recover_stdio()//dev/tty是终端控制台
{
    int ttyfd;
    ttyfd = open("/dev/tty",O_RDONLY);
    dup2(ttyfd,0);
    dup2(ttyfd,1);
    dup2(ttyfd,2);
}
//标准的输入输出流分为三种
//>>函数可使用O_APPEND标识符来进行追加操作
void output_redirce(char *filename,int mode)
{
    printf("filename %s \n",filename);

    if(filename==NULL)
        return ;
    int fd;
    if(mode == 1)
     fd = open(filename,O_RDWR | O_APPEND |O_CREAT , 0644 );
     else
     fd = open(filename,O_WRONLY | O_CREAT , 0644);
    if( fd< 0)
        perror("open error");
    dup2(fd,1);
    close(fd);
    return;
}
static void intput_redirce(char * filename)
{
    int fd;
    fd=open(filename,O_RDWR);
    if(fd < 0)
        perror("< error");
    dup2(fd,0);
    close(fd);
    return ;
}
//因为将命令传递给execv函数需要去掉
//执行命令
void do_cmd(char *list[])
{
    param.backgroud = 0;
    //如果命令中有&,表示后台运行,父进程直接返回,不等子进程，
    int status;
    int mode=0;
    pid_t pid;
    int j = 0;
    int flag=0;//标记有没有特殊字符，比如重定向或者&
    //查看有没有后台运行程序
    for (j = 0; j < i; j++)
    {
        if (strncmp(list[j], "&", 1) == 0)
        {
            if (j <= i - 1)
            {
                param.backgroud = 1;
                break;
            }
            if (j > i - 1)
            {
                perror("Wrong command\n");
                return;
            }
        }
        if (strcmp(list[j], "|") == 0)
        {
            flag=1;
            param.have_pipe += 3;
        }
        if (strcmp(list[j], ">") == 0)
        {     
            flag=1;
              pid=fork();
            if(pid==0)
            {
                param.out_redirect = 1; 
               list[j]=NULL;  
               output_redirce(list[j+1],mode);
               find_command(list);
                exit(0);
            }
            break;
        }
        if (strcmp(list[j], "<") == 0)
        {
            flag=1;
            pid=fork();
            if(pid == 0)
            {
                param.in_redirect = 1;
                list[j]=NULL;
                intput_redirce(list[j+1]);
                find_command(list);
                exit(0);
            }
            break;
        }
        if (strcmp(list[j], ">>") == 0)
        {
            flag=1;
            pid=fork();
            if(pid == 0)
            {
                param.out_redirects = 1;
                list[j]=NULL;
                output_redirce(list[j+1],1);
                find_command(list);
                exit(0);
            }
            break;
        }
        if(strcmp(list[j],"&") == 0)
        {
            param.backgroud=1;
        }
    }
    if(flag == 0)
    {
        pid=fork();
        if(pid == 0)
         find_command(list);
    }
     if (param.backgroud == 1) //如果命令中有后台执行的程序
    {
        pid=fork();
        if(pid == 0)
            find_command(list);
        else
            printf("(process id %d )\n", pid);
            return;
    }
     if (waitpid(pid, &status, 0) == -1)
    {
        printf("wait for child process error\n");
    }
}
void showhistroy()
{
    HIST_ENTRY **history;
    history = history_list();
    int k = 0;
    while (history[k] != NULL)
    {
        printf("%s\n", history[k]->line);
        k++;
    }
}
//查找命令中的可执行命令，在这里应该fork一个子进程让它在后台，不断工作到终止
//不然的话执行一个进程就会死掉
void find_command(char *path[])
{
    int pid;
    int child_info = -1;
    if (*path == NULL)
        return;
    if (strcmp(*path, "exit") == 0 || strcmp(*path, "logout") == 0)
        exit(0);
    if ((pid = fork()) == -1)
        perror("fork");
    else if (pid == 0)
    {
        execvp(path[0], path);
    }
    else
    {
        if (wait(&child_info) == -1)
            perror("wait");
    }
    return;
}
//cd内置命令
void shell_cd(char *path[])
{ 
    struct passwd *usrname; ///头文件在pwd.h之中，需要添加
    usrname = getpwuid(getuid());
    if( strcmp(path[1],"~")==0 )
    {
          if(strcmp(usrname->pw_name,"kiosk")==0 )
          {
               strcpy(path[1],"/home/kiosk/");
          }
          if(strcmp(usrname->pw_name,"root")==0)
          {
              strcpy(path[1],"/root/");
          }
    }
    if( strcmp(path[1],"-")==0 )
    {
        strcpy(path[1],oldpwd[o]);
    }
    getcwd(oldpwd[o], sizeof(oldpwd[o]) - 1); //保存绝对路径
    //printf("%s\n",oldpwd[o]);
    o++;
    if (chdir(path[1]) < 0) //chdir可以改变当前的工作目录，fchdir用来将当前目录改为由文件描述符所指定的目录
    {
        perror("cd");
    }
}
int main(int argc, char **argv)
{
    char *buf = NULL;
    buf = (char *)malloc(MAXARGS * ARGLEN);
    char *list[256];
    *list = (char *)malloc(256);
    if (buf == NULL)
    {
        perror("malloc failed");
        exit(-1);
    }
    /* signal(SIGINT, SIG_IGN);//屏蔽掉信号
        signal(SIGQUIT, SIG_IGN);
        signal(SIGSTOP, SIG_IGN);
        signal(SIGTSTP, SIG_IGN);*/
    while (1)
    {
       read_history(NULL);
        memset(buf, 0, 1200);
        memset(list, 0, sizeof(&list));
        get_ifnput(buf);
        if (strcmp(buf, "exit") == 0 || strcmp(buf, "logout") == 0)
            break;
        if (strcmp(buf, "\n") == 0)
            continue; //多个回车
        explain(buf, list);
        if (strcmp(list[0], "cd") == 0)
        {
            shell_cd(list);
        }
        if (strcmp(list[0], "history") == 0)
            showhistroy();
        do_cmd(list);
        //find_command(list);
    }
    free(buf);
    for (i = 0; i < 256; i++) //将二位数组进行释放
        free(list[i]);
    exit(0);
}
