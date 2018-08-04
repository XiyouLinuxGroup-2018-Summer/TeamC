#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdio.h>

#include<stdlib.h>
/*查看所有守护进程的命令：
ps -eo ppid,pid,sid,stat,tty,comm  | awk '{ if ($2 == $3 && $5 == "?") {print $0}; }'
*/
// 守护进程初始化函数
void init_daemon()
{
    pid_t pid;
    int i = 0;
    if ((pid = fork()) == -1) {
        printf("Fork error !\n");
        exit(1);
    }
    if (pid != 0) {
        exit(0);        /*父进程退出*/
    }
    /* 子进程部分 */
    setsid();           // 子进程开启新会话，并成为会话首进程和组长进程
    chdir("/tmp");      // 改变工作目录
    umask(0);           // 重设文件掩码
    for (; i < getdtablesize(); ++i) {
        close(i);        // 关闭打开的文件描述符
    }
    return ;
}
int main(int argc, char *argv[])
{
    int fp;
    time_t t;
    char buf[] = { "This is a daemon:  "};
    char *datetime;
    int len = 0;

    // 初始化 Daemon 进程
    init_daemon();
    // 每隔一分钟记录运行状态
    while (1) {
        if (-1 == (fp = open("/tmp/daemon.log", O_CREAT|O_WRONLY|O_APPEND, 0600))) {
            printf("Open file error !\n");
            exit(1);
        }
        len = strlen(buf);
        write(fp, buf, len); //在/tmp/daemon.log文件中写入buf,即This is a deamon :
        t = time(0); 
        datetime = asctime(localtime(&t));
        len = strlen(datetime);
        write(fp, datetime, len); //写入时间
        close(fp); 
        sleep(5);
    }
    return 0;
}
