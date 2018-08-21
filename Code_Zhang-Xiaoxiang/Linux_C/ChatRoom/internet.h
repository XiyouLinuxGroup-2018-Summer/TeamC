#ifndef _INTERNET_H
#define _INTERNET_H

// 周知端口: 0到1023 监听需要root权限
// 动态端口: 1024到65535
// server/client configure
#define PORT    10240                   // 端口号
#define SIP     "127.0.0.1"             // 本地先凑合用
#define CIP     "127.0.0.1"             // 凑合....

// server: map<int id>
// int32_t: 防止因机器不同导致的传输解析问题
typedef struct package
{
    int32_t cmdflag;                    // 命令标志
    int32_t statusflag;                 // 发送给人or群
    int32_t source_id;                  // 发送方id
    int32_t target_id;                  // 操作对象 | 无操作对象设为-1
    char strmsg[MSG_SIZE + 1];      // 发送的信息
}Package;

// ------------------------------------------------------------------------------------------------------------
// statusflag
#define Flag_Status_user    0       // 发送对象为用户   
#define Flag_Status_Group   1       // 发送对象为群组1
// cmdflag
#define Flag_Cmd_Empty      0       // pre-for Hardware interruption(备用)
#define Flag_Cmd_Msg        1       // 发送的是聊天消息, strmsg 只存储消息
#define Flag_Cmd_Login      2       // 登录操作 strmsg format: userid+\r\t\n+pass\r\t\n        wait server return 
#define Flag_Cmd_Register   3       // 注册操作 strmsg format: username + \r\t\n + pass\r\t\n  ...
#define Flag_Cmd_RePass     4       // 找回密码 strmsg format: question + \r\t\n + answer\r\t\n ...
#define Flag_Cmd_AddFri     5       // 添加好友 strmsg format: \r\t\n[空]
#define Flag_Cmd_DelFri     6       // 删除好友 strmsg format: \r\t\n[空]
#define Flag_Cmd_LkFriList  7       // 查看好友列表 strmsg format: \r\t\n 
#define Flag_Cmd_ChatToOne  8       // 私聊请求 strmsg format: 空
#define Flag_Cmd_ChatToGrp  9       // 群聊请求 strmsg format: 空
#define Flag_Cmd_LkChatRec  10      // 查看聊天记录 strmsg format: 空
#define Flag_Cmd_ShiSome    11      // 屏蔽某人消息 strmsg format: 空
#define Flag_Cmd_CreateGrp  12      // 创建群聊 strmsg format: 空
#define Flag_Cmd_AddGrp     13      // 添加群聊 strmsg format: 空
#define Flag_Cmd_QuitGrp    14      // 退出群聊
#define Flag_Cmd_LkGrpMem   15      // 查看群成员
#define Flag_Cmd_DisBand    16      // 解散群成员
#define Flag_Cmd_SetCtrl    17      // 设置管理员
#define Flag_Cmd_UnSetCtrl  18      // 取消管理员
#define Flag_Cmd_RemvSome   19      // 将某人移出群
#define Flag_Cmd_SendFile   20      // 发送文件
#define Flag_Cmd_RecvFile   21      // 接受文件
#define Flag_Cmd_InvAddFri  22      // 同意添加好友
#define Flag_Cmd_InvAddGrp  23      // 同意好友邀请加群
#define Flag_Cmd_InvAddMem  24      // [管理/群主]同意加群申请
#define Flag_Cmd_Exit       -1      // 客户端退出

// -------------------------------------------------------------------------------------------------------------
int PACK_SIZE = sizeof(Package);
// 封装send(), 防止黏包
int SendMSG(int socket, const Package msg, int PACK_SIZE, int flags);
// 封装recv(), 防止黏包
int RecvMSG(int socket, const Package msg, int PACK_SIZE, int flags);

#endif
