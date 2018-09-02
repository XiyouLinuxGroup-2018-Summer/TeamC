#ifndef _INTER_NET_H
#define _INTER_NET_H

// server/client configure
#define PORT    10240                   // 端口号
#define SIP     "127.0.0.1"             // 本地先凑合用
#define CIP     "127.0.0.1"             // 凑合....

// 消息种类
#define MSG_FriNOR          0        // 私聊信息
#define MSG_GRPNOR          -1       // 群聊信息
#define MSG_SYS_RECVFILE    -2       // 系统信息: 接受文件
// #define MSG_SYS_INVIGRP     -3       // 系统信息: 好友邀群
#define MSG_SYS_INVIFri     -4       // 系统信息: 加好友请求
#define MSG_SYS_AGRAGRP     -5       // 系统信息: 加群请求


// int32_t: 防止因机器不同导致的传输解析问题
typedef struct package
{
    int32_t cmdflag;                    // 命令标志
    int32_t statusflag;                 // 消息类型
    int32_t source_id;                  // 发送方id
    int32_t target_id;                  // 操作对象 | 无操作对象设为-1
    char strmsg[MSG_SIZE];              // 发送的信息
}Package;

#define PACK_SIZE sizeof(Package)

// server to client [cmdflag or strmsg]
// 服务器给客户端的回执标记在statusflag, cmdflag意义不变
#define Flag_Status_user    0       // 发送对象为用户   
#define Flag_Status_Group   1       // 发送对象为群组

// cmdflag
#define Flag_Cmd_Empty      0       // pre-for Hardware interruption(备用)
#define Flag_Cmd_Msg        1       // 发送的是聊天消息, strmsg 只存储消息
#define Flag_Cmd_Login      2       // 登录操作 strmsg format: userid+\r\t\n+pass\r\t\n   
#define Flag_Cmd_Register   3       // 注册操作 strmsg format: username + \r\t\n +
#define Flag_Cmd_UnShiSome  4       // 解除屏蔽好友
#define Flag_Cmd_AddFri     5       // 添加好友 strmsg format: [空]
#define Flag_Cmd_DelFri     6       // 删除好友 strmsg format: [空]
#define Flag_Cmd_LkFriList  7       // 查看好友列表 strmsg format: [空]  
#define Flag_Cmd_LkGrpList  8       // 查看群聊列表
#define Flag_Cmd_SerIfInGrp 9       // 判断某人是否在群组里面
#define Flag_Cmd_ShiSome    10      // 屏蔽某人消息 strmsg format: [空]
#define Flag_Cmd_CreateGrp  11      // 创建群聊 strmsg format: owner_id, name, something
#define Flag_Cmd_AddGrp     12      // 添加群聊 
#define Flag_Cmd_QuitGrp    13      // 退出群聊
#define Flag_Cmd_LkGrpMem   14      // 查看群成员
#define Flag_Cmd_DisBand    15      // 解散群
#define Flag_Cmd_SetCtrl    16      // 设置管理员
#define Flag_Cmd_UnSetCtrl  17      // 取消管理员
#define Flag_Cmd_RemvSome   18      // 将某人移出群
#define Flag_Cmd_SendFile   19      // 发送文件
#define Flag_Cmd_RecvFile   20      // 接受文件
#define Flag_Cmd_InvAddFri  21      // 同意添加好友
#define Flag_Cmd_InvAddGrp  22      // 同意好友邀请加群
#define Flag_Cmd_InvAddMem  23      // [管理/群主]同意加群申请
#define Flag_Cmd_SyncOffMsg 24      // 离线消息同步
#define Flag_Cmd_Exit       -1      // 客户端退出

// 封装send(), 防止黏包
int SendMSG(int socket, const Package* msg, int psize, int flags);
// 封装recv(), 防止黏包
int RecvMSG(int socket, Package* msg, int psize, int flags);

#endif
