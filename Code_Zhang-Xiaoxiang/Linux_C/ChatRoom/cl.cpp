// ChatRoom_client
#include "base.h"
#include "view.h"
#include "internet.h"
#include <queue>
#include <vector>
using namespace std;

#define PATH "./.MSG"  

typedef struct fri
{
    int id;
    char name[USER_NAME_MAX + 1];
    int shield;
}sFriend;
typedef struct grp 
{
    int id;
    char name[USER_NAME_MAX + 1];
    int shield;
}sGroup;

typedef struct message                  // 消息结构
{                                       // sourceid 即为消息类型
    int  sourceid;
    int  targetid;
    int  kind;                          // 标记消息来源，相同来源的消息放入同一个vector,
    char msg[MSG_SIZE + 1];             
    char msgtime[25];                   // 存放发送消息的时间
}Message;

int flag = 0;                                           // 标记，recv获取到信息之后才会处理相关事情
int ChatWithWho = 0;                                    // 标记和谁在聊天 -　默认设置为0, 值为用户id
int CmdExecSta = -1;                                    // 操作执行情况，0-默认，1 -> 成功, -1 -> 失败

int user_id, sock_fd;                                   // 用户id                                     
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;      // 新建一个锁，防止读写冲突
vector<sFriend>myFriend;                                // 好友列表
vector<sGroup>myGroup;                                  // 群组列表
vector<Message>MessageBox;                              // 消息盒子

void* PrecvMSG(void * arg);                             // 线程: 接受消息
void  vMsgBox(void);                                    // 消息盒子
void* InitiaClient(void * arg);                                   // 初始化客户端
void GetOffMSG(void);
void SendGetFriList(void);
void GetFriList(int num);
void PrintFriList(void);
int  SearchMyFri(int searchid);
void FunFriendMenu(int tarid);
void ChatWith(int tarid, int kind);
void ShiSome(int tarid);
void UnShiSome(int tarid);
int  MainMenu(void);
void MessageHistory(int tarid);
void DeleteSome(int tarid);
void AddFri(int tarid);
void LookMsgBox();

int main(void)
{
    signal(SIGPIPE, SIG_IGN);                           // 屏蔽sigpipe信号

    int ret, loopflag = 1, status = 0;                  // status: 登录状态 loopflag: 循环标识 ret: 函数返回值处理  
    pthread_t Trecv, Tinit;                             // 线程id
    struct sockaddr_in cli_addr;                        // 套接字结构
    Package Datapack;                                   // 数据包

    memset(&cli_addr, 0, sizeof(struct sockaddr_in));
    cli_addr.sin_family = AF_INET;                      // 设置为tcp/ip     
    cli_addr.sin_port = htons(PORT);                    // 设置端口
    cli_addr.sin_addr.s_addr = inet_addr(SIP);          // 设置ip

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);          // 创建套接字
    if (sock_fd < 0)
        my_err(__FILE__, "sock", __LINE__, 0);

    // 连接服务器
    ret = connect(sock_fd, (struct sockaddr*)&cli_addr, sizeof(struct sockaddr_in));
    if (ret < 0)
        my_err(__FILE__, "connect", __LINE__, 0);

    // 开始界面
    while (loopflag)
    {
        Menu:ret = vBeginRLQ();
        while (getchar() != '\n');
        switch(ret)
        {
            case 1:// 注册
                // send package to server
                ret = vRegister(&Datapack, sock_fd);
                if (ret == 0)
                    printf("注册失败, 请稍后再试\n");
                else    // 注册成功
                {
                    status = 1;         // 标记为已登录
                    user_id = ret;
                    printf(":) 注册成功\n");
                    printf("你的用户id为：%d\n", ret);
                    printf("请牢记你的用户id和密码，用户id将会是你的唯一身份标识～\n");
                    ret = pthread_create(&Tinit, NULL, InitiaClient, NULL);
                    if (ret < 0)
                        my_err(__FILE__, "pthread_create", __LINE__, 0);
                    for (int i = 10; i > 0; i--)
                    {
                        printf("%2d 秒后按回车键继续", i);
                        fflush(stdout);
                        sleep(1);
                        printf("\r");
                    }
                    while (getchar() != '\n')
                        ;
                    printf("\n");
                    loopflag = 0;
                }
                break;
            case 2:// 登录
                ret = vLogin(&Datapack, sock_fd);
                if (ret == 0)
                {
                    printf("登录失败！\n");
                    sleep(1);
                }
                else
                {
                    status = 1;
                    user_id = ret;
                    printf(":) 登录成功\n");
                    ret = pthread_create(&Tinit, NULL, InitiaClient, (void*)&sock_fd);
                    if (ret < 0)
                        my_err(__FILE__, "pthread_create", __LINE__, 0);
                    for (int i = 3; i > 0; i--)
                    {
                        printf("%d 秒后按任意键继续", i);
                        fflush(stdout);
                        sleep(1);
                        printf("\r");
                    }
                    printf("\n");
                    loopflag = 0;
                }
                break;
            case 3:// 退出
                close(sock_fd);
                pthread_mutex_destroy(&mutex);
                exit(0);
                break;
            case 4:// 关于
                vAbout();
                break;
        }
    }

    pthread_join(Tinit, NULL);                          // 初始化
    ret = pthread_create(&Trecv, NULL, PrecvMSG, (void*)&sock_fd);    
    if (ret < 0)
        my_err(__FILE__, "pthread_create", __LINE__, 0);

    int begain = 1;
    while (1)
    {
        S_CLEAR();
        if (begain)
        {
            if (!MessageBox.empty())
                printf("你有新的消息，请到消息盒子处理\n");
            begain = 0;
        }
        ret = MainMenu();
        switch (ret)
        {
            case 1: 
                LookMsgBox();
                break;
            case 2:
                while (1)
                {
                    SendGetFriList();
                    PrintFriList();
                    if (myFriend.size())
                    {
                        int cmd;
                        printf("请选择要操作的用户id:(0 quit)\n");
                        if (scanf("%d", &cmd) != 1 || (cmd ==  0))
                        {
                            while (getchar() != '\n')
                                ;
                            break;
                        }
                        FunFriendMenu(cmd);
                    }
                    else
                        break;
                }
                break;
            case 3:
                break;
            case 4:
                int addid;
                printf("请输入要添加的好友id:\n");
                if (scanf("%d", &addid) != 1)
                {
                    while (getchar() != '\n')
                        ;
                    break;
                }
                AddFri(addid);
                break;
            case 5:
                break;
            case 6:
                break;
            case 7:
                status = 0;
                goto Menu;
                break;
            case 8: 
                close(sock_fd);
                return 0;
                break;
            default:
                printf("无效命令\n");
                break;
        }
    }

    return 0;
}

void* PrecvMSG(void * arg)
{
    int ret, kind;
    Package recvpack;
    while (1)
    {
        ret = RecvMSG(sock_fd, &recvpack, PACK_SIZE, 0);
        if (ret == 0)
        {
            fprintf(stderr, "服务器维护中...\n");
            exit(0);
        }
        if (ret < 0)
            my_err(__FILE__, "RecvMSG", __LINE__, 0);

        kind = recvpack.cmdflag;
        switch (kind)
        {
            case Flag_Cmd_LkFriList:
            {
                pthread_mutex_lock(&mutex);
                printf("===========\n");
                GetFriList(recvpack.statusflag);
                flag = 1;                       // 标记事件已完成
                pthread_mutex_unlock(&mutex);
                break;
            }
            case Flag_Cmd_Msg:
            {
                if (recvpack.source_id == ChatWithWho)      // 正在聊天
                {
                    get_time(NULL);
                    S_COLOR(49, 33);
                    printf("%s", recvpack.strmsg);
                    S_CLOSE();
                    printf("\n");
                }
                else                                        // 放入消息盒子
                {
                    Message m;
                    m.kind = recvpack.statusflag;
                    strcpy(m.msg, recvpack.strmsg);
                    get_time(m.msgtime);
                    m.sourceid = recvpack.source_id;
                    m.targetid = recvpack.target_id;
                    MessageBox.push_back(m);

                    printf("[system] 你有一条新消息，请到消息盒子查看\n");
                }
                break;
            }
            // case Flag_
        }   
    }
}

void* InitiaClient(void * arg)     
{
    // 清空数据，防止注销登录另一个账号导致的数据混乱
    MessageBox.clear();
    myFriend.clear();
    myGroup.clear();
    // 发送请求，获取离线消息
    // GetOffMSG(); 

    pthread_exit(0);
}

void GetOffMSG(void)
{
    int ret, num;
    Package sendpack, recvpack;
    memset(&sendpack, 0, sizeof(sendpack));
    sendpack.source_id = user_id;
    sendpack.cmdflag = Flag_Cmd_SyncOffMsg;

    ret = SendMSG(sock_fd, &sendpack, PACK_SIZE, 0);    // 发送请求
    if (ret < 0)
        my_err(__FILE__, "SendMSG", __LINE__, 0);

    ret = RecvMSG(sock_fd, &recvpack, __LINE__, 0);     // 接受包的数量
    if (ret < 0)
        my_err(__FILE__, "RecvMSG", __LINE__, 0);
    if (ret == 0)
    {
        fprintf(stderr, "服务器未响应!!\n");
        my_err(__FILE__, "RecvMSG", PACK_SIZE, 0);
    }
    num = recvpack.statusflag;
    printf("%d\n", num);

    for (int i = 0; i < num; i++)
    {
        ret = RecvMSG(sock_fd, &recvpack, PACK_SIZE, 0);
        printf("ret = %d\n", ret);
        if (ret < 0)
            my_err(__FILE__, "SendMSG", __LINE__, 0);
        if (ret == 0)
        {
            fprintf(stderr, "服务器未响应!!\n");
            my_err(__FILE__, "RecvMSG", PACK_SIZE, 0);
        }

        // 解析包
        char * str1, * str2, *str3, * str4, * str6;
        char tempid[12], temptarid[12], tempstatus[5], tempmsg[128], temptime[30];
        str1 = strstr(recvpack.strmsg, _END_);
        str2 = strstr(str1 + strlen(_END_), _END_);
        str3 = strstr(str2 + strlen(_END_), _END_);
        str4 = strstr(str3 + strlen(_END_), _END_);
        str6 = strstr(str4 + strlen(_END_), _END_);

        strncpy(tempid, recvpack.strmsg, str1 - recvpack.strmsg);
        tempid[str1 - recvpack.strmsg] = '\0';
        strncpy(temptarid, str1 + strlen(_END_), str2 - str1 - strlen(_END_));
        temptarid[str2 - str1 - strlen(_END_)] = '\0';
        strncpy(tempstatus, str2 + strlen(_END_), str3 - str2 - strlen(_END_));
        tempstatus[str3 - str2 - strlen(_END_)] = '\0';
        strncpy(tempmsg, str3 + strlen(_END_), str4 - str3 - strlen(_END_));
        tempmsg[str4-str3 - strlen(_END_)] = '\0';
        strncpy(temptime, str3 + strlen(_END_), str4 - str3 - strlen(_END_));
        temptime[str4-str3 - strlen(_END_)] = '\0';

        Message temp;
        temp.sourceid = atoi(tempid);
        temp.targetid = atoi(temptarid);
        temp.kind = atoi(tempstatus);
        strcpy(temp.msg, tempmsg);
        strcpy(temp.msgtime, temptime);

        MessageBox.push_back(temp);
    }

}

void SendGetFriList(void)
{
    int ret;
    Package sendpack;
    memset(&sendpack, 0, sizeof(sendpack));
    sendpack.cmdflag = Flag_Cmd_LkFriList;      
    sendpack.source_id = user_id;       
    ret = SendMSG(sock_fd, &sendpack, PACK_SIZE, 0);
    if (ret < 0)
        my_err(__FILE__, "SendMSG", __LINE__, 0);
    while (flag == 0)  
        ;
    flag = 0;
}

void GetFriList(int num)
{
    int ret;
    Package recvpack;
    myFriend.clear();       // 清空以存储

    for (int i = 0; i < num; i++)
    {
        ret = RecvMSG(sock_fd, &recvpack, PACK_SIZE, 0);
        if (ret < 0)
            my_err(__FILE__, "SendMSG", __LINE__, 0);
        if (ret == 0)
        {
            fprintf(stderr, "服务器未响应!!\n");
            my_err(__FILE__, "RecvMSG", __LINE__, 0);
        }
        
        // 解析
        char * str1, * str2, *str3;
        char tempid[12], tempname[USER_NAME_MAX + 1], tempshi[5];
        str1 = strstr(recvpack.strmsg, _END_);
        str2 = strstr(str1 + strlen(_END_), _END_);
        str3 = strstr(str2 + strlen(_END_), _END_);
        strncpy(tempid, recvpack.strmsg, str1 - recvpack.strmsg);
        tempid[str1 - recvpack.strmsg] = '\0';
        strncpy(tempname, str1 + strlen(_END_), str2 - str1 - strlen(_END_));
        tempname[str2 - str1 - strlen(_END_)] = '\0';
        strncpy(tempshi, str2 + strlen(_END_), str3 - str2 - strlen(_END_));
        tempshi[str3 - str2 - strlen(_END_)] = '\0';

        sFriend m;
        m.id = atoi(tempid);
        strcpy(m.name, tempname);
        m.shield = atoi(tempshi);

        myFriend.push_back(m);                  // 放入vector
    }
}

void SendGetGrpList(void)
{
    int ret;
    Package sendpack;
    memset(&sendpack, 0, sizeof(sendpack));
    sendpack.cmdflag = Flag_Cmd_LkGrpList;      
    sendpack.source_id = user_id;       
    ret = SendMSG(sock_fd, &sendpack, PACK_SIZE, 0);
    if (ret < 0)
        my_err(__FILE__, "SendMSG", __LINE__, 0);
    while (flag == 0)  
        ;
    flag = 0;
}

void GetGrpList(int num)
{
    int ret;
    Package recvpack;
    myFriend.clear();       // 清空以存储

    for (int i = 0; i < num; i++)
    {
        ret = RecvMSG(sock_fd, &recvpack, PACK_SIZE, 0);
        if (ret < 0)
            my_err(__FILE__, "SendMSG", __LINE__, 0);
        if (ret == 0)
        {
            fprintf(stderr, "服务器未响应!!\n");
            my_err(__FILE__, "RecvMSG", __LINE__, 0);
        }
        
        // 解析
        char * str1, * str2, *str3;
        char tempid[12], tempname[USER_NAME_MAX + 1], tempshi[5];
        str1 = strstr(recvpack.strmsg, _END_);
        str2 = strstr(str1 + strlen(_END_), _END_);
        str3 = strstr(str2 + strlen(_END_), _END_);
        strncpy(tempid, recvpack.strmsg, str1 - recvpack.strmsg);
        tempid[str1 - recvpack.strmsg] = '\0';
        strncpy(tempname, str1 + strlen(_END_), str2 - str1 - strlen(_END_));
        tempname[str2 - str1 - strlen(_END_)] = '\0';
        strncpy(tempshi, str2 + strlen(_END_), str3 - str2 - strlen(_END_));
        tempshi[str3 - str2 - strlen(_END_)] = '\0';

        sGroup m;
        m.id = atoi(tempid);
        strcpy(m.name, tempname);
        m.shield = atoi(tempshi);

        myGroup.push_back(m);                  // 放入vector
    }
}

void PrintFriList()
{
    int num = (int)myFriend.size();             // 获取大小
    if (num == 0)
        printf("好友列表为空\n");
    for (int i = 0; i < num; i++)
        printf("[%d] '%s' %s\n", myFriend[i].id, myFriend[i].name, myFriend[i].shield ? "×" : " ");
}

void PrintGrpList()
{
    int num = (int)myGroup.size();             // 获取大小
    if (num == 0)
        printf("群组列表为空\n");
    for (int i = 0; i < num; i++)
        printf("[%d] '%s' %s\n", myGroup[i].id, myGroup[i].name, myGroup[i].shield ? "×" : " ");
}

int SearchMyFri(int searchid)
{
    int num = (int)myFriend.size();
    for (int i = 0; i < num; i++)
    {
        if (myFriend[i].id == searchid)
            return i;
    }
    return 0;
}

void FunFriendMenu(int tarid)
{
    int cmd;
    if (SearchMyFri(tarid) == 0)
        printf("他还不是你的好友!\n");
    do
    {
        S_CLEAR(); 
        printf("                  - Function -                   \n\n");    
        printf("\t\t[1] 和ta聊天\t\t\t\n");
        printf("\t\t[2] 查看聊天记录\t\t\t\n");
        printf("\t\t[3] 加入黑名单\t\t\t\n");
        printf("\t\t[4] 移出黑名单\t\t\t\n");
        printf("\t\t[5] 删除联系人\t\t\t\n");
        printf("\t\t[6] 发送文件\t\t\t\n");
        // printf("\t\t[7] 返回\t\t\t\n");
        printf("\n-------------------------------------------------\n");
        printf("\nYour Choice is :\t");
        scanf("%d", &cmd);
        while (getchar() != '\n');
        if (cmd > 0 && cmd <= 7)
            break;
    }while(1); 

    switch (cmd)
    {
        case 1:
            ChatWithWho = tarid;
            ChatWith(tarid, 1);
            ChatWithWho = 0;
            break;
        case 2:
            MessageHistory(tarid);
            break;
        case 3:
            ShiSome(tarid);
            break;
        case 4:
            UnShiSome(tarid);
            break;
        case 5:
            DeleteSome(tarid);
        case 6:
            // SendFile(tarid);
            break;
        // case 7:
        //     break;
        default:
            printf("无效命令\n");
    }
}

void FunGroupMenu()

// kind = 1, 私聊
// kind = 0, 群聊
void ChatWith(int tarid, int kind)
{
    S_CLEAR(); 
    int ret;
    char inputmsg[128];
    Package Msg;
    printf("                  - Chat with %d -                   \n\n", tarid);    
    while (1)
    {
        while (s_gets(inputmsg, 510, stdin) && inputmsg[0] !='\0')
        {
            if (strcmp(inputmsg, "quit") == 0)
                return;
            Msg.source_id = user_id;
            Msg.target_id = tarid;
            Msg.cmdflag = Flag_Cmd_Msg;
            strcpy(Msg.strmsg, inputmsg);
            if (kind)
                Msg.statusflag = MSG_FriNOR;
            else   
                Msg.statusflag = MSG_GRPNOR;

            ret = SendMSG(sock_fd, &Msg, PACK_SIZE, 0);
            if (ret < 0)
                my_err(__FILE__, "SendMSG", __LINE__, 0);
        }
    }
}

void ShiSome(int tarid)
{
    int ret;
    int pos = SearchMyFri(tarid);
    Package sendpack;
    if (myFriend[pos].shield == 1)
    {
        printf("好友已被屏蔽，无须操作\n");
        return ;
    }
    sendpack.cmdflag = Flag_Cmd_ShiSome;
    sendpack.source_id = user_id;
    sendpack.target_id = tarid;
    myFriend[pos].shield = 1;

    ret = SendMSG(sock_fd, &sendpack, PACK_SIZE, 0);
    if (ret < 0)
        my_err(__FILE__, "SendMSG", __LINE__, 0);
    printf("好友已被屏蔽，你将不会收到来自他的信息\n");
}

void UnShiSome(int tarid)
{
    int ret;
    int pos = SearchMyFri(tarid);
    Package sendpack;
    if (myFriend[pos].shield == 0)
    {
        printf("好友没有被屏蔽，无须操作\n");
        return ;
    }
    sendpack.cmdflag = Flag_Cmd_UnShiSome;
    sendpack.source_id = user_id;
    sendpack.target_id = tarid;
    myFriend[pos].shield = 0;

    ret = SendMSG(sock_fd, &sendpack, PACK_SIZE, 0);
    if (ret < 0)
        my_err(__FILE__, "SendMSG", __LINE__, 0);
    printf("已经解除对好友的屏蔽\n");
}

int MainMenu()
{
    int cmd;
    do
    {
        printf("                  - Function -                   \n\n");    
        printf("\t\t[1] 查看消息盒子\t\t\t\n");
        // [system] && [friend and group]
        printf("\t\t[2] 查看好友列表\t\t\t\n");
        printf("\t\t[3] 查看群组列表\t\t\t\n");
        printf("\t\t[4] 添加好友\t\t\t\n");
        printf("\t\t[5] 添加群聊\t\t\t\n");
        printf("\t\t[6] 创建群聊\t\t\t\n");
        printf("\t\t[7] 退出登录\t\t\t\n");
        printf("\t\t[8] 退出软件\t\t\t\n");
        printf("\n-------------------------------------------------");
        printf("\nYour Choice is :\t");
        scanf("%d", &cmd);
        while (getchar() != '\n');
        if (cmd > 0 && cmd <= 8)
            return cmd;
    }while(1);   
}

void MessageHistory(int tarid)
{
    // block;
}

void DeleteSome(int tarid)
{
    int ret;
    Package sendpack;
    sendpack.cmdflag = Flag_Cmd_DelFri;
    sendpack.source_id = user_id;
    sendpack.target_id = tarid;

    ret = SendMSG(sock_fd, &sendpack, PACK_SIZE, 0);
    if (ret < 0)
        my_err(__FILE__, "SendMSG", __LINE__, 0);
    
    // 更新好友列表
    SendGetFriList();
}

void AddFri(int tarid)
{
    int ret;
    Package sendpack;
    if (tarid == user_id)
    {
        printf("无效操作！无法添加自已为好友\n");
        return ;
    }
    if (SearchMyFri(tarid) != 0)
    {
        printf("他已经是你的好友了，请不要重复添加！\n");
        return ;
    }
    sendpack.cmdflag = Flag_Cmd_AddFri;
    sendpack.source_id = user_id;
    sendpack.target_id = tarid;
    
    ret = SendMSG(sock_fd, &sendpack, PACK_SIZE, 0);
    if (ret < 0)
        my_err(__FILE__, "SendMSG", __LINE__, 0);

    printf("添加好友申请已发送\n");
    return ;
}

void LookMsgBox()
{
    int choice, cmd, ret;
    int num = MessageBox.size();
    Package sendpack;
    S_CLEAR();
    printf("                  - MessageBox -                   \n\n");
    for (int i = 0; i < num; i++)
    {
        printf("[%d]\t", i + 1);
        if (MessageBox[i].kind != MSG_FriNOR || MessageBox[i].kind != MSG_GRPNOR)
            printf("[System Message]\t");
        else    
            printf("[%s'%d'给你发来一条信息]\t", MessageBox[i].kind == MSG_FriNOR ? "用户" : "群聊", MessageBox[i].sourceid);
        printf("%s\n", MessageBox[i].msgtime);
        printf("\t%s\n", MessageBox[i].msg);
    }
    while(1)
    {
        printf("\n请选择要处理的系统消息, 无效选择将会退出\n");
        printf("[Your Choice]\t");
        if (scanf("%d", &choice) != 1)
        {
            while (getchar() != '\n');
            return ;
        }
        if (choice <= 0 || choice > num)
            return;
        if (MessageBox[choice - 1].kind == MSG_FriNOR)
        {
            ChatWith(MessageBox[choice - 1].sourceid, 1);
        }
        else if (MessageBox[choice - 1].kind == MSG_GRPNOR)
        {
            ChatWith(MessageBox[choice - 1].sourceid, 0);
        }
        else if (MessageBox[choice - 1].kind == MSG_SYS_INVIFri)
        {
            printf("是否同意添加对方为好友？(1/0)\n");
            scanf("%d", &cmd);
            if (cmd)
            {
                memset(&sendpack, 0, sizeof(sendpack));
                strcpy(sendpack.strmsg, MessageBox[choice - 1].msg);
                sendpack.cmdflag = Flag_Cmd_InvAddFri;
                sendpack.source_id = MessageBox[choice - 1].sourceid;
                sendpack.target_id = MessageBox[choice - 1].targetid;

                printf("source_id = %d\n", sendpack.source_id);
                printf("target_id = %d\n", sendpack.target_id);

                ret = SendMSG(sock_fd, &sendpack, PACK_SIZE, 0);
                // printf("success\n");
                if (ret < 0)
                    my_err(__FILE__, "SendMSG", __LINE__, 0);
                printf("success\n");
            }
        }
        else if (MessageBox[choice - 1].kind == MSG_SYS_AGRAGRP)
        {
            printf("是否同意对方加群？(1/0)\n");
            scanf("%d", &cmd);
            if (cmd)
            {
                memset(&sendpack, 0, sizeof(sendpack));
                strcpy(sendpack.strmsg, MessageBox[choice - 1].msg);
                sendpack.cmdflag = Flag_Cmd_InvAddMem;
                sendpack.source_id = MessageBox[choice - 1].sourceid;
                sendpack.source_id = MessageBox[choice - 1].targetid;
                ret = SendMSG(sock_fd, &sendpack, PACK_SIZE, 0);
                if (ret < 0)
                    my_err(__FILE__, "SendMSG", __LINE__, 0);
            }
        }
        // 文件
    }
}
