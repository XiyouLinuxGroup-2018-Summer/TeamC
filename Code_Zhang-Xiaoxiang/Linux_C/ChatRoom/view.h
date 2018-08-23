#ifndef _VIEW_H_
#define _VIEW_H_

#include "base.h"
#include "internet.h"

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

// fun: 找回密码
// int vForgetPass(char * question, char * answer);

// fun: 对好友操作
int vFunFriendMenu(void);

// // fun: 对群成员的操作
// int vFunMemMenu(void);

// // fun: 对群组操作
// int vFunGroupMenu(void);

// // fun: SendFile
// int vSendFile(void);

// // fun: 查看好友列表
// int vLookFriendList(void);

// // fun: 查看群组列表
// int vLookGroupList(void);

// // fun: 添加好友
// int vAddFriend(void);

// // fun: 添加群聊
// int vAddGroup(void);

// // fun: 创建群聊
// int vCreateGroup(void);

// // fun: 消息盒子
// // note:
// //  - 系统消息
// //      1. 是否接受发来的离线文件
// //      2. 是否同意好友发来的邀请加群请求
// //      3. 是否同意加好友的请求
// //      4. if 群主 or 管理员
// //              是否接受有人的加群请求
// //      5. [群主]/[管理员]是否同意加群请求
// //  - 普通群聊、私聊信息
// //      询问查看哪一条信息， 打开聊天界面
// void vMsgBox(void);

#endif
