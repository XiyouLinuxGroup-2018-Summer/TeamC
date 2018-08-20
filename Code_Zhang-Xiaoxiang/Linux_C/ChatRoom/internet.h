#ifndef _INTERNET_H
#define _INTERNET_H

// server: map<int id>
typedef struct package
{
    int cmdflag;                    // 命令标志
    int statusflag;                 // 发送给人or群
    int source_id;                  // 发送方id
    int target_id;                  // 操作对象 | 无操作对象设为-1
    char strmsg[MSG_SIZE + 1];      // 发送的信息
}Package;

// statusflag
#define Flag_Status_user    0       // 发送对象为用户   
#define Flag_Status_Group   1       // 发送对象为群组
#define Flag_Cmd_Empty      0       // pre-for Hardware interruption(备用)
#define Flag_Cmd_Msg        1       // 发送的是聊天消息, strmsg 只存储消息
#define Flag_Cmd_Login      2       // 登录操作 strmsg format: userid+\r\t\n+pass\r\t\n        wait server return 
#define Flag_Cmd_Register   3       // 注册操作 strmsg format: username + \r\t\n + pass\r\t\n  ...

int PACK_SIZE = sizeof(Package);

// 封装send(), 防止黏包
int SendMSG(int socket, const Package msg, int PACK_SIZE, int flags);
// 封装recv(), 防止黏包
int RecvMSG(int socket, const Package msg, int PACK_SIZE, int flags);

#endif
