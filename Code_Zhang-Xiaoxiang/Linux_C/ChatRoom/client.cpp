// ChatRoom_client
#include "base.h"
#include "view.h"
#include "internet.h"
#include <queue>
#include <vector>
using namespace std;

#define PATH "./.MSG"    
// Directory: 存放聊天记录
// 目录下文件命名格式： PATH/[f/g]+[userid/groupid].txt

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

// struct cmp
// {
//     bool operator()(vector<Message> &a, vector<Message> &b) const
//     {
//         return strcmp(a.back().msgtime, b.back().msgtime);
//     }
// };

int MsgNum = 0;
int ChatWithWho = 0;                                    // 标记和谁在聊天 -　默认设置为0, 值为用户id
int CmdExecSta;                                     // 操作执行情况，0-默认，1 -> 成功, -1 -> 失败
int user_id;                                        // 用户id                                     // 操作执行情况，0-默认，1 -> 成功, -1 -> 失败
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;      // 新建一个锁，防止读写冲突
int sock_fd;   

vector<sFriend>myFriend;
vector<sGroup>myGroup;
// priority_queue<vector<Message>, vector<Message>, cmp>MessageBox;    
vector<Message>MessageBox;   

void* PrecvMSG(void * arg);                             // 线程: 接受消息
void  vMsgBox(void);                                    // 消息盒子
void* InitiaClient(void * arg);                         // 初始化客户端
int  vFriendList(void);
void GetFriList(int sockfd);
void GetGroupList(int sockfd);
void vMsgBox(int sockfd);
void vChatWithOne(int tarid);
void fun_exit(int sig)
{
    close(sock_fd);
    exit(0);
}
int main(void)
{
    signal(SIGPIPE, SIG_IGN);                           // 屏蔽sigpipe信号
    signal(SIGINT, fun_exit);

    int ret, loopflag = 1, status = 0;                  // status: 登录状态 loopflag: 循环标识 ret: 函数返回值处理  
    char recvbuf[BUF_SIZE], sendbuf[BUF_SIZE];          // 设置缓冲区
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
                    ret = pthread_create(&Tinit, NULL, InitiaClient, (void*)&sock_fd);
                    if (ret < 0)
                        my_err(__FILE__, "pthread_create", __LINE__, 0);
                    for (int i = 10; i > 0; i--)
                    {
                        printf("%2d 秒后按任意键继续", i);
                        fflush(stdout);
                        sleep(1);
                        printf("\r");
                    }
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

    pthread_join(Tinit, NULL);                          // 等待初始化操作完成
    ret = pthread_create(&Trecv, NULL, PrecvMSG, (void*)&sock_fd);    // 创建线程接受服务器传来的消息
    loopflag = 1;
    if (ret < 0)
        my_err(__FILE__, "pthread_create", __LINE__, 0);
    
    while (1)
    {
        MsgNum = MessageBox.size();
        vMessageTop(MsgNum);                            // 消息提示
        printf("\n\n\n");
        ret = vFunMainMenu();
        switch(ret)
        {
            case 1: // 查看消息盒子
                vMsgBox(sock_fd);
                MessageBox.clear();                     // 清空消息盒子 
                MsgNum = 0;
                break;
            case 2: // 查看好友列表
                while(1)
                {
                    int kase = vFriendList();
                    if (kase == -1)
                        break;
                    ret = vFunFriendMenu();
                    switch(ret)
                    {
                        case 1:
                        {
                            ChatWithWho = myFriend[kase].id;
                            vChatWithOne(ChatWithWho);
                            break;
                        }
                        case 2:
                        {
                            break;
                        }
                        case 3:
                        {
                            break;
                        }
                        case 4:
                        {
                            break;
                        }
                        case 5:
                        {
                            break;
                        }
                        case 6:
                        {
                            break;
                        }
                        case 7:
                        {
                            break;
                        }
                    }
                }
                break;
            case 3: // 查看群组列表
                break;
            case 4: // 添加好友
            {
                S_CLEAR(); S_BLOD(); S_COLOR(40, 33);
                printf("                  - AddFriend -                   \n\n");    
                S_CLOSE();
                printf("\n输入你要添加的好友id: \n");
                scanf("%d", &ret);

                Datapack.source_id = user_id;
                Datapack.target_id = ret;
                Datapack.cmdflag = Flag_Cmd_AddFri;
                
                ret = SendMSG(sock_fd, &Datapack, PACK_SIZE, 0);
                if (ret < 0)
                    my_err(__FILE__, "SendMSG", __LINE__, 0);
                printf("添加好友请求已发送:)\n");

                break;
            }
            case 5: // 添加群聊
                break;
            case 6: // 创建群聊
                break;
            case 7: // 退出登录
                status = 0;
                // 清理相关文件
                goto Menu;
                break;
            case 8:
                // 处理内存泄露、结束线程等
                break;
        }
    }
    
    return 0;
}

void* PrecvMSG(void * arg)
{
    int ret, kase, kind, source;
    int sockfd = *(int*)arg;
    Package recvpack;
    while (1)
    {
        ret = RecvMSG(sockfd, &recvpack, PACK_SIZE, 0);
        if (ret == 0)
        {
            fprintf(stderr, "服务器维护中...\n");
            exit(0);
        }
        if (ret < 0)
            my_err(__FILE__, "RecvMSG", __LINE__, 0);
        kase = recvpack.cmdflag;
        switch(kase)
        {
            case Flag_Cmd_Msg:
            {
                source = recvpack.source_id;
                if (source == ChatWithWho)            // 如果正在聊天
                {
                    get_time(NULL);
                    S_COLOR(S_DEFAULT_BACK, 33);
                    printf("%s\n", recvpack.strmsg);
                    S_CLOSE();
                }
                else                                    // 放入消息盒子
                {
                    Message m;
                    m.kind = Flag_Cmd_Msg;
                    strcpy(m.msg, recvpack.strmsg);
                    get_time(m.msgtime);
                    m.sourceid = source;
                    m.targetid = recvpack.target_id;
                    MessageBox.push_back(m);
                }
                break;
            }
        }
    }
}

void* InitiaClient(void * socketfd)     
{
    // 初始化好友列表，群组列表，离线消息
    int num, ret;
    int sockfd = *(int*)socketfd;
    Package recvpack, sendpack;

    // 清空数据，防止注销登录另一个账号导致的数据混乱
    myFriend.clear();
    myGroup.clear();
    MessageBox.clear();
    // while (!MessageBox.empty())
    //     MessageBox.pop();

    // 发送请求，获取离线消息
    memset(&sendpack, 0, sizeof(sendpack));
    sendpack.source_id = user_id;
    sendpack.cmdflag = Flag_Cmd_SyncOffMsg;
    ret = SendMSG(sockfd, &sendpack, PACK_SIZE, 0);
    if (ret < 0)
        my_err(__FILE__, "SendMSG", __LINE__, 0);
    ret = RecvMSG(sockfd, &recvpack, PACK_SIZE, 0);
    if (ret < 0)
        my_err(__FILE__, "SendMSG", __LINE__, 0);
    if (ret == 0)
    {
        fprintf(stderr, "服务器未响应!!\n");
        my_err(__FILE__, "RecvMSG", PACK_SIZE, 0);
    }
    num = recvpack.statusflag;

    for (int i = 0; i < num; i++)
    {
        ret = RecvMSG(sockfd, &recvpack, PACK_SIZE, 0);
        if (ret < 0)
            my_err(__FILE__, "SendMSG", __LINE__, 0);
        if (ret == 0)
        {
            fprintf(stderr, "服务器未响应!!\n");
            my_err(__FILE__, "RecvMSG", PACK_SIZE, 0);
        }
        char * str1, * str2, *str3, *str4, *str5;
        char tempid[12], temptarid[12], tempstatus[5], tempmsg[128], temptime[30];
        str1 = strstr(recvpack.strmsg, _END_);
        str2 = strstr(str1 + strlen(_END_), _END_);
        str3 = strstr(str2 + strlen(_END_), _END_);
        str4 = strstr(str3 + strlen(_END_), _END_);
        str5 = strstr(str4 + strlen(_END_), _END_);

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

        MessageBox[i].sourceid = atoi(tempid);
        MessageBox[i].targetid = atoi(temptarid);
        MessageBox[i].kind = atoi(tempstatus);
        strcpy(MessageBox[i].msg, tempmsg);
        strcpy(MessageBox[i].msgtime, temptime);
    }

    pthread_exit(0);
}

void vMsgBox(int sockfd)
{
    S_CLEAR(); S_BLOD(); S_COLOR(40, 33);
    printf("                  - MessageBox -                   \n\n");    
    S_CLOSE();
    if (MessageBox.empty())
        printf("\t你现在没有消息\n");
    else
    {
        int choice, cmd, ret;
        int num = MessageBox.size();
        Package sendpack;
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
            scanf("%d", &choice);
            if (MessageBox[choice - 1].kind == MSG_FriNOR || MessageBox[choice - 1].kind == MSG_GRPNOR)
                return;
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
                    sendpack.source_id = MessageBox[choice - 1].targetid;
                    ret = SendMSG(sockfd, &sendpack, PACK_SIZE, 0);
                    if (ret < 0)
                        my_err(__FILE__, "SendMSG", __LINE__, 0);
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
                    ret = SendMSG(sockfd, &sendpack, PACK_SIZE, 0);
                    if (ret < 0)
                        my_err(__FILE__, "SendMSG", __LINE__, 0);
                }
            }
            // 文件
        }
    }

}

void GetGroupList(int sockfd)
{
    int num, ret;
    Package recvpack, sendpack;
    // 发送请求，获取群组列表
    memset(&sendpack, 0, sizeof(sendpack));
    sendpack.source_id = user_id;
    sendpack.cmdflag = Flag_Cmd_LkGrpList;
    ret = SendMSG(sockfd, &sendpack, PACK_SIZE, 0);
    if (ret < 0)
        my_err(__FILE__, "SendMSG", __LINE__, 0);
    ret = RecvMSG(sockfd, &recvpack, PACK_SIZE, 0);
    if (ret < 0)
        my_err(__FILE__, "SendMSG", __LINE__, 0);
    if (ret == 0)
    {
        fprintf(stderr, "服务器未响应!!\n");
        my_err(__FILE__, "RecvMSG", PACK_SIZE, 0);
    }
    num = recvpack.statusflag;
    for (int i = 0; i < num; i++)
    {
        ret = RecvMSG(sockfd, &recvpack, PACK_SIZE, 0);
        if (ret < 0)
            my_err(__FILE__, "SendMSG", __LINE__, 0);
        if (ret == 0)
        {
            fprintf(stderr, "服务器未响应!!\n");
            my_err(__FILE__, "RecvMSG", PACK_SIZE, 0);
        }
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

        myGroup[i].id = atoi(tempid);
        strcpy(myGroup[i].name, tempname);
        myGroup[i].shield = atoi(tempshi);
    }
}

void GetFriList(int sockfd)
{
    int num, ret;
    Package recvpack, sendpack;
    // 发送请求，获取好友列表
    memset(&sendpack, 0, sizeof(sendpack));
    sendpack.source_id = user_id;
    sendpack.cmdflag = Flag_Cmd_LkFriList;
    ret = SendMSG(sockfd, &sendpack, PACK_SIZE, 0);
    if (ret < 0)
        my_err(__FILE__, "SendMSG", __LINE__, 0);
    ret = RecvMSG(sockfd, &recvpack, PACK_SIZE, 0);
    if (ret < 0)
        my_err(__FILE__, "SendMSG", __LINE__, 0);
    if (ret == 0)
    {
        fprintf(stderr, "服务器未响应!!\n");
        my_err(__FILE__, "RecvMSG", PACK_SIZE, 0);
    }
    num = recvpack.statusflag;
    for (int i = 0; i < num; i++)
    {
        ret = RecvMSG(sockfd, &recvpack, PACK_SIZE, 0);
        if (ret < 0)
            my_err(__FILE__, "SendMSG", __LINE__, 0);
        if (ret == 0)
        {
            fprintf(stderr, "服务器未响应!!\n");
            my_err(__FILE__, "RecvMSG", PACK_SIZE, 0);
        }
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

        myFriend[i].id = atoi(tempid);
        strcpy(myFriend[i].name, tempname);
        myFriend[i].shield = atoi(tempshi);
    }
}

int vFriendList()
{
    int num, cmd, ret;
    S_CLEAR(); 
    // S_BLOD(); S_COLOR(40, 33);
    printf("                  - FriendList -                   \n\n");    
    // S_CLOSE();
    GetFriList(sock_fd);

    num = myFriend.size();
    if (num == 0)
    {
        printf("好友列表为空\n");
        return -1;
    }
        
    while (1)
    {
        for (int i = 0; i < num; i++)
        {
            printf("[%d] %s(%d)\t", i + 1, myFriend[i].name, myFriend[i].id);
            if (myFriend[i].shield)
            {
                S_COLOR(S_DEFAULT_BACK, 31);
                printf("×");
                S_CLOSE();
            }
            printf("\n");
        }
        printf("\n\n请选择要操作的好友[序号]:\t");
        scanf("%d", &cmd);
        if (cmd <= 0 || cmd > num)
            continue;
        else    
            return cmd;  
    }
}

int vGroupList()
{
    int num, cmd, ret;
    S_CLEAR(); 
    // S_BLOD(); S_COLOR(40, 33);
    printf("                  - GroupList -                   \n\n");    
    // S_CLOSE();
    GetGroupList(sock_fd);

    num = myGroup.size();
    if (num == 0)
    {
        printf("好友列表为空\n");
        return -1;
    }

    while (1)
    {
        for (int i = 0; i < num; i++)
        {
            printf("[%d] %s(%d)\t", i + 1, myGroup[i].name, myGroup[i].id);
            if (myGroup[i].shield)
            {
                S_COLOR(S_DEFAULT_BACK, 31);
                printf("×");
                S_CLOSE();
            }
            printf("\n");
        }
        printf("\n\n请选择要操作的群[序号]:\t");
        scanf("%d", &cmd);
        if (cmd <= 0 || cmd > num)
            continue;
        else    
            return cmd - 1;  
    }
}

void vChatWithOne(int m)
{
    S_CLEAR(); S_BLOD(); S_COLOR(40, 33);
    printf("                  - GroupList -                   \n\n");    
    S_CLOSE();
    return;
}
