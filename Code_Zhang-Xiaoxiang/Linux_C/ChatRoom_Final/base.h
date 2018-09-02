#ifndef _CR_BASE_H_
#define _CR_BASE_H_

// head file
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <errno.h>
#include <termio.h>
#include <mysql/mysql.h>
#include <pthread.h>

// 屏幕光标
// #define CLEAR()           printf("\ec")              // 清屏
#define CLEAR()             printf("\033[2J")
#define CLOSE()             printf("\e[0m")            // 关闭所有属性
#define BLOD()              printf("\e[1m")            // 强调、加粗、高亮
#define FLCK()              printf("\e[5m")            // 闪烁
#define UNDER()             printf("\e[4m")            // 下划线
#define REVERSE()           printf("\e[7m")            // 反显
#define CREND()             printf("\e[K")             // 清除从光标到行尾的内容
#define SAVEPOS()           printf("\e[s")             // 保存光标位置
#define RESTPOS()           printf("\e[u")             // 恢复光标位置
#define S_COLOR(x,y)        printf("\e["#x";"#y"m")    // x: 背景，y: 前景
#define S_SETPOS(x,y)       printf("\e["#y";"#x"H")    // 设置光标位置
#define S_UPROW(x)          printf("\e["#x"A")         // 光标上移x行
#define S_DWROW(x)          printf("\e["#x"B")         // 光标下移x行
#define S_RGCOL(x)          printf("\e["#x"C")         // 光标右移x列
#define S_LFCOL(x)          printf("\e["#x"D")         // 光标左移x列

#define COLOR(x, y)         S_COLOR(x,y)
#define SETPOS(x, y)        S_SETPOS(x,y)
#define UPROW(x)            S_UPROW(x)
#define DWROW(x)            S_DWROW(x)
#define RGCOL(x)            S_RGCOL(x)
#define LFCOL(x)            S_LFCOL(x)

// color 
#define S_DEFAULT_FORE      39                         // 默认前景色
#define S_DEFAULT_BACK      49                         // 默认背景色
#define fBLACK              30
#define fRED                31
#define fGREEN              32
#define fYELLOW             33
#define fBLUE               34
#define fPURPLE             35
#define fDGREEN             36
#define fWHITE              37
#define bBLACK              40
#define bRED                41
#define bGREEN              42
#define bYELLOW             43
#define bBLUE               44
#define bPURPLE             45
#define bDGREEN             46
#define bWHITE              47

// limits
#define USER_NAME_MAX       12
#define USER_PASS_MIN       6
#define USER_PASS_MAX       10
#define MSG_SIZE            512         // 信息长度
#define BUF_SIZE            2048        // 缓冲区大小
#define OTHER_SIZE          256         // 杂项
#define MEM_NUM             256         // 群成员限制
#define FRI_NUM             256         // 好友上限
#define GRP_NUM             100         // 加群上限
#define OffMsg_NUM          99          // 离线消息上限

#define _END_               "\r\t\n"    // empty msg or failed

// functions
int getch(void);
void get_time(char * string);
char* s_gets(char * st, int n, FILE *fp);
char* s_getchs(char * st, int n);
void my_err(const char * file, const char * err_string, int line);

#endif
