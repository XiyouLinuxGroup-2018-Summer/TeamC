#ifndef _VIEW_H_
#define _VIEW_H_

#include "../Base/base.h"
#include "../Base/internet.h"

// fun: 注册登录退出关于界面
// 成功返回对应的选项
int vBeginRLQ(void);

// fun: 注册界面
// arg->data: 发送给服务器的数据包
// arg->sockfd: 发送注册请求的套接字
// return: 返回值同 exec_mysql_fun
int vRegister(Package * data, int sockfd);

// fun: 关于界面
// note: 调用函数后，输入任意字符返回最开始界面(vBeginRLQ())
void vAbout(void);

// fun: 登录之后第一个主界面显示有无离线消息的消息框
// arg->num: 消息的数量
void vMessageTop(int num);

// fun: 登录之后的主功能列表
// return: 返回选择的功能
int vFunMainMenu(void);

// fun: 登录
int vLogin(Package * data, int sockfd);

#endif
