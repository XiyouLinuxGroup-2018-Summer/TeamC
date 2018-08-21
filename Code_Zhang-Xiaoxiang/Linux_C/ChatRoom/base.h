/* 
    基础函数即宏定义
*/
#ifndef _BASE_H_
#define _BASE_H_

// include head file -------------------------------------------------------
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

// 与屏幕光标有关的宏定义 ----------------------------------------------------
#define S_CLEAR()           printf("\ec")              // 清屏
#define S_CLOSE()           printf("\e[0m")            // 关闭所有属性
#define S_BLOD()            printf("\e[1m")            // 强调、加粗、高亮
#define S_FLCK()            printf("\e[5m")            // 闪烁
#define S_UNDER()           printf("\e[4m")            // 下划线
#define S_REVERSE()         printf("\e[7m")            // 反显
#define S_COLOR(x,y)        printf("\e["#x";"#y"m")    // x: 背景，y: 前景
#define S_SETPOS(x,y)       printf("\e["#y";"#x"H")    // 设置光标位置
#define S_UPROW(x)          printf("\e["#x"A")         // 光标上移x行
#define S_DWROW(x)          printf("\e["#x"B")         // 光标下移x行
#define S_RGCOL(x)          printf("\e["#x"C")         // 光标右移x列
#define S_LFCOL(x)          printf("\e["#x"D")         // 光标左移x列
#define S_CREND()           printf("\e[K")             // 清除从光标到行尾的内容
#define S_SAVEPOS()         printf("\e[s")             // 保存光标位置
#define S_RESTPOS()         printf("\e[u")             // 恢复光标位置

#define S_DEFAULT_FORE      39                         // 默认前景色
#define S_DEFAULT_BACK      49                         // 默认背景色

// ========================= just temp view =============================
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

// 错误处理 ---------------------------------------------------------------
void my_err(const char * err_string, int line);

// input -----------------------------------------------------------------
int getch(void);
void get_time(char * string);
char * s_gets(char * st, int n, FILE *fp);
char* s_getchs(char * st, int n);

// Limits ----------------------------------------------------------------
#define USER_NAME_MAX       12
#define USER_PASS_MIN       6
#define USER_PASS_MAX       10
#define MSG_SIZE            1024        // 信息长度
#define OTHER_SIZE          256         // 杂项

// some ------------------------------------------------------------------
#define _END_               "\r\t\n"    // empty msg or failed

#endif
