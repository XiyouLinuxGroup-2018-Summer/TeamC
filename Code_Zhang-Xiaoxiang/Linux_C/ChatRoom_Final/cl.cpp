// ChatRoom_client
#include "base.h"
#include "view.h"
#include "internet.h"
#include <queue>
#include <vector>
using namespace std;

typedef struct fri
{
    int id;
    char name[USER_NAME_MAX + 1];
    int shield;
    int status;                         // 好友在线状态
}sFriend;
typedef struct grp 
{
    int id;
    char name[USER_NAME_MAX + 1];
    int shield;
}sGroup;
typedef struct mem
{
    int id;
    int status;
}sMember;

typedef struct message                  // 消息结构
{                                       // sourceid 即为消息类型
    int  sourceid;
    int  targetid;
    int  kind;                          // 标记消息来源，相同来源的消息放入同一个vector,
    char msg[MSG_SIZE + 1];             
    char msgtime[25];                   // 存放发送消息的时间
}Message;

static int flag = 0;                                           // 标记，recv获取到信息之后才会处理相关事情
static int ChatWithWho = 0;                                    // 标记和谁在聊天 -　默认设置为0, 值为用户id 群聊为负值，私聊为正值
static int user_id, sock_fd;                                    // 用户id                                     
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;       // 新建一个锁，防止读写冲突
static vector<sFriend>myFriend;                                 // 好友列表
static vector<sGroup>myGroup;                                   // 群组列表
static vector<Message>MessageBox;                               // 消息盒子
static vector<sMember>GrpMem;

void* PrecvMSG(void * arg);                                     // 线程: 接受消息
void* InitiaClient(void * arg);                                   // 初始化客户端
void GetOffMSG(void);
void SendGetFriList(void);
void GetFriList(Package recvpack);
void PrintFriList(void);
int  SearchMyFri(int searchid);
void FunFriendMenu(int tarid);
void ChatWith(int tarid, int kind);
void ShiSome(int tarid);
void UnShiSome(int tarid);
int  MainMenu(void);
void MessageHistory(int tarid, int kind);
void DeleteSome(int tarid);
void AddFri(int tarid);
void LookMsgBox(void);
void LookMember(int grpid);
void ExGroup(int grpid, int user);
void ConGroup(int grpid);
void ExGroup(int grpid, int user);
void RemoveSome(int grpid);
void DisBand(int grpid);
void UnSetConMem(int grpid);
void SetConMem(int grpid);
int  SearchInGroup(int userid);
void GetMemberList(Package recvpack);
void Exit(void);
void FunGroupMenu(int grpid);
void AddGrp(int id);
void CreateGrp(void);
void GetGrpList(Package recvpack);
void SendGetGrpList(void);
void PrintGrpList(void);
int SearchMyGrp(int searchid);
void PrintGrpMem(void);
void SendFile(int tarid);

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
        my_err(__FILE__, "sock", __LINE__);

    // 连接服务器
    ret = connect(sock_fd, (struct sockaddr*)&cli_addr, sizeof(struct sockaddr_in));
    if (ret < 0)
    {
        close(sock_fd);
        my_err(__FILE__, "connect", __LINE__);
    }

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
                        my_err(__FILE__, "pthread_create", __LINE__);
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
                    {
                        close(sock_fd);
                        my_err(__FILE__, "pthread_create", __LINE__);
                    }
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
    {
        close(sock_fd);
        my_err(__FILE__, "pthread_create", __LINE__);
    }

    int begain = 1;
    while (1)
    {
        int addid;        
        CLEAR();
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
                SendGetGrpList();
                if (myGroup.size() == 0)
                    break;
                else
                {
                    while (1)
                    {
                        int cmd;
                        PrintGrpList();
                        printf("请输入要操作的群id:(0 quit)\n");
                        if (scanf("%d", &cmd) != 1 || (cmd ==  0))
                        {
                            while (getchar() != '\n')
                                ;
                            break;
                        }
                        FunGroupMenu(cmd);
                    }
                }
                break;
            case 4:
                printf("请输入要添加的好友id:\n");
                if (scanf("%d", &addid) != 1)
                {
                    while (getchar() != '\n')
                        ;
                    break;
                }
                AddFri(addid);
                break;
            case 5: // 添加群
                printf("请输入要添加的群id:\n");
                if (scanf("%d", &addid) != 1)
                {
                    while (getchar() != '\n')
                        ;
                    break;
                }
                AddGrp(addid);
                break;
            case 6:
                CreateGrp();
                break;
            case 7:
                status = 0;
                goto Menu;
                break;
            case 8: 
                Exit();
                return 0;
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
        {
            close(sock_fd);
            my_err(__FILE__, "RecvMSG", __LINE__);
        }

        kind = recvpack.cmdflag;
        switch (kind)
        {
            case Flag_Cmd_LkFriList:
            {
                pthread_mutex_lock(&mutex);
                if (recvpack.statusflag == -111)
                    flag = 1;                       // 标记事件已完成
                else 
                    GetFriList(recvpack);
                pthread_mutex_unlock(&mutex);
                break;
            }
            case Flag_Cmd_Msg:
            {
                // printf("recvpack.source_id = %d\n", recvpack.source_id);
                // printf("chatwithwho = %d\n", ChatWithWho);
                char tempfile[30];
                if (ChatWithWho < 0)
                    sprintf(tempfile, "./%d", -1 * ChatWithWho);
                else
                    sprintf(tempfile, "./%dand%d", user_id, ChatWithWho);
                
                int fd = open(tempfile, O_RDWR | O_CREAT | O_APPEND, S_IRWXU | S_IRGRP);
                if (fd == -1)
                    perror("open");

                char msg[MSG_SIZE + 1];
                get_time(msg);
                strcat(msg, recvpack.strmsg);
                strcat(msg, "\n");

                // printf("msg = %s\n", msg);
                write(fd, msg, strlen(msg));

                if (recvpack.source_id == ChatWithWho || recvpack.source_id == -1 * ChatWithWho)      
                {
                    get_time(NULL);
                    COLOR(49, 33);
                    printf("%s\n", recvpack.strmsg);
                    CLOSE();
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

                    printf("\n[system] 你有一条新消息，请到消息盒子查看\n");
                }

                close(fd);

                break;
            }
            case Flag_Cmd_CreateGrp:
            {
                int ret = recvpack.statusflag;
                if (ret)
                {
                    printf("群聊创建成功 :)\n");
                    printf("群id为%d:\n", ret);
                }
                else 
                {
                    printf("Sorry, 服务器出了点故障，群聊创建失败 :< \n");
                }
                break;
            }
            case Flag_Cmd_LkGrpList:
            {
                pthread_mutex_lock(&mutex);
                if (recvpack.statusflag == -111)
                    flag = 1;
                else 
                    GetGrpList(recvpack);
                pthread_mutex_unlock(&mutex);
                break;
            }
            case Flag_Cmd_LkGrpMem:
            {
                pthread_mutex_lock(&mutex);
                if (recvpack.statusflag == -111)
                    flag = 1;                       // 标记事件已完成
                else 
                    GetMemberList(recvpack);
                pthread_mutex_unlock(&mutex);
                break;
            }
            case Flag_Cmd_SendFile: // 有文件
            {
                Message fmsg;
                fmsg.kind = MSG_SYS_RECVFILE;
                fmsg.sourceid = recvpack.source_id;
                fmsg.targetid = recvpack.target_id;
                strcpy(fmsg.msg, recvpack.strmsg);
                get_time(fmsg.msgtime);

                MessageBox.push_back(fmsg);
                printf("\n[system] 你有一条新消息，请到消息盒子查看\n");
                break;
            }
            case Flag_Cmd_RecvFile: // 接收文件
            {
                int fd, ret; 
                long long len, all;
                char filename[256], * str;
                
                sprintf(filename, "./%d_%d", recvpack.source_id, recvpack.target_id);

                if (recvpack.statusflag == -222)
                {
                    fd = creat(filename, S_IRWXU);          // 创建文件
                    if (fd == -1)
                    {
                        perror("create file failed");
                        break;
                    }
                    close(fd);
                }
                else if (recvpack.statusflag == -111)
                {
                    printf("文件接收完成!\n");
                    sleep(2);
                }
                else 
                {
                    fd = open(filename, O_WRONLY | O_APPEND);     // 追加
                    if (fd == -1)
                    {
                        perror("open");
                        break;
                    }
                    all = 0;
                    str = recvpack.strmsg;
                    while (1)
                    {
                        len = write(fd, str, (size_t)(recvpack.statusflag - all));
                        all += len;
                        if (all == recvpack.statusflag)
                            break;
                    }
                    close(fd);
                }

                break;
            }
        }   
    }
}

void* InitiaClient(void * arg)     
{
    // 清空数据，防止注销登录另一个账号导致的数据混乱
    MessageBox.clear();
    myFriend.clear();
    myGroup.clear();
    GrpMem.clear();
    // 发送请求，获取离线消息
    GetOffMSG(); 

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
    {
        close(sock_fd);
        my_err(__FILE__, "SendMSG", __LINE__);
    }

    ret = RecvMSG(sock_fd, &recvpack, PACK_SIZE, 0);     // 接受包的数量
    if (ret < 0)
    {
        close(sock_fd);
        my_err(__FILE__, "RecvMSG", __LINE__);
    }

    if (ret == 0)
    {
        fprintf(stderr, "服务器未响应!!\n");
        my_err(__FILE__, "RecvMSG", PACK_SIZE);
    }
    num = recvpack.statusflag;
    // printf("%d\n", num);

    for (int i = 0; i < num; i++)
    {
        ret = RecvMSG(sock_fd, &recvpack, PACK_SIZE, 0);
        // printf("ret = %d\n", ret);
        // printf("msg: %s\n", recvpack.strmsg);
        if (ret < 0)
        {
            close(sock_fd);
            my_err(__FILE__, "SendMSG", __LINE__);
        }
        if (ret == 0)
        {
            fprintf(stderr, "服务器未响应!!\n");
            close(sock_fd);
            my_err(__FILE__, "RecvMSG", PACK_SIZE);
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
    myFriend.clear();
    memset(&sendpack, 0, sizeof(sendpack));

    sendpack.cmdflag = Flag_Cmd_LkFriList;      
    sendpack.source_id = user_id;       
    ret = SendMSG(sock_fd, &sendpack, PACK_SIZE, 0);
    if (ret < 0)
    {
        close(sock_fd);
        my_err(__FILE__, "SendMSG", __LINE__);
    }
 
    while (flag == 0)  
        ;
    flag = 0;
}

void GetFriList(Package recvpack)
{
    // 解析
    char * str1, * str2, * str3, * str4;
    char tempid[12], tempname[USER_NAME_MAX + 1], tempshi[5], tempsta[3];
    str1 = strstr(recvpack.strmsg, _END_);
    str2 = strstr(str1 + strlen(_END_), _END_);
    str3 = strstr(str2 + strlen(_END_), _END_);
    str4 = strstr(str3 + strlen(_END_), _END_);
    strncpy(tempid, recvpack.strmsg, str1 - recvpack.strmsg);
    tempid[str1 - recvpack.strmsg] = '\0';
    strncpy(tempname, str1 + strlen(_END_), str2 - str1 - strlen(_END_));
    tempname[str2 - str1 - strlen(_END_)] = '\0';
    strncpy(tempshi, str2 + strlen(_END_), str3 - str2 - strlen(_END_));
    tempshi[str3 - str2 - strlen(_END_)] = '\0';
    strncpy(tempsta, str3 + strlen(_END_), str4 - str3 - strlen(_END_));
    tempsta[str4 - str3 - strlen(_END_)] = '\0';

    sFriend m;
    m.id = atoi(tempid);
    strcpy(m.name, tempname);
    m.shield = atoi(tempshi);
    m.status = atoi(tempsta);

    myFriend.push_back(m);                  // 放入vector
}

void SendGetGrpList(void)
{
    int ret;
    Package sendpack;
    myGroup.clear();       
    memset(&sendpack, 0, sizeof(sendpack));

    sendpack.cmdflag = Flag_Cmd_LkGrpList;      
    sendpack.source_id = user_id;       
    ret = SendMSG(sock_fd, &sendpack, PACK_SIZE, 0);
    if (ret < 0)
    {
        close(sock_fd);
        my_err(__FILE__, "SendMSG", __LINE__);
    }

    while (flag == 0)  
        ;
    flag = 0;
}

void GetGrpList(Package recvpack)
{
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

void PrintFriList()
{
    int num = (int)myFriend.size();             // 获取大小
    if (num == 0)
        printf("好友列表为空\n");
    for (int i = 0; i < num; i++)
        printf("[%d] '%s' %s %s\n", myFriend[i].id, myFriend[i].name, myFriend[i].shield ? "×" : " ", myFriend[i].status ? "在线" : "离线");
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
    return -1;
}

int SearchMyGrp(int searchid)
{
    int num = (int)myGroup.size();
    for (int i = 0; i < num; i++)
    {
        if (myGroup[i].id == searchid)
            return i;
    }
    return -1;
}

void FunFriendMenu(int tarid)
{
    int cmd, loop = 1, temp;
    if (SearchMyFri(tarid) == -1)
        printf("他还不是你的好友!\n");
    do
    {
        CLEAR(); 
        printf("                  - Function -                   \n\n");    
        printf("\t\t[1] 和ta聊天\t\t\t\n");
        printf("\t\t[2] 查看聊天记录\t\t\t\n");
        printf("\t\t[3] 加入黑名单\t\t\t\n");
        printf("\t\t[4] 移出黑名单\t\t\t\n");
        printf("\t\t[5] 删除联系人\t\t\t\n");
        printf("\t\t[6] 发送文件\t\t\t\n");
        printf("\t\t[7] 返回\t\t\t\n");
        printf("\n-------------------------------------------------\n");
        printf("\nYour Choice is :\t");
        scanf("%d", &cmd);
        while (getchar() != '\n');
        switch (cmd)
        {
            case 1:
                ChatWithWho = tarid;
                ChatWith(tarid, 1);
                ChatWithWho = 0;
                break;
            case 2:
                MessageHistory(tarid, 1);
                break;
            case 3:
                ShiSome(tarid);
                break;
            case 4:
                UnShiSome(tarid);
                break;
            case 5:
                printf("请输入要删除的好友id:\n");
                scanf("%d", &temp);
                DeleteSome(temp);
                loop = 0;
                break;
            case 6:
                SendFile(tarid);
                break;
            case 7:
                loop = 0;
                break;
            default:
                printf("无效命令\n");
        }
    }while(loop); 
}

void SendFile(int tarid)
{
    char Path[256], filename[64], * str, * find;
    Package sendpack;
    long long off, ret, len, all;

    printf("请输入你要发送文件的绝对路径:\n");
    fgets(Path, 255, stdin);
    find = strchr(Path, '\n');          // 除去换行符
    if (find)
        *find = '\0';
    str = strrchr(Path, '/');
    if (!str)
    {
        printf("无效的路径！\n");
        return ;
    }
    sprintf(filename, "%s", str + 1);   // 从文件中获取文件名

    int fd = open(Path, O_RDONLY);      // 只读方式打开文件
    if (fd == -1)
    {
        printf("文件打开失败！请检查路径是否正确和文件权限!\n");
        perror("open");
        return;
    }

    off = lseek(fd, 0, SEEK_END);

    // 发送包传递包的大小和文件名称，告诉服务器创建一个文件
    sendpack.cmdflag = Flag_Cmd_SendFile;
    sendpack.source_id = user_id;
    sendpack.target_id = tarid;
    sendpack.statusflag = -222;             // 标记开始
    strcpy(sendpack.strmsg, filename);      // 存储文件名
    ret = SendMSG(sock_fd, &sendpack, PACK_SIZE, 0);
    if (ret < 0)
    {
        printf("服务器出错!\n");
        perror("SendMSG");
        close(sock_fd);
        exit(1);
    }    

    lseek(fd, 0, SEEK_SET);                 // 重置文件指针

    printf(">> 包的大小为：%lld\n", off);
// for debug --- 
    char tempcmd[300];
    sprintf(tempcmd, "md5sum %s", Path);
    system(tempcmd);
// end ---------

    all = 0;                                // 记录读取到的字节数
    while (all < off)
    {
        sendpack.cmdflag = Flag_Cmd_SendFile;
        sendpack.source_id = user_id;
        sendpack.target_id = tarid;          

        int need;
        if (off - all > sizeof(sendpack.strmsg))
            need = sizeof(sendpack.strmsg);
        else 
            need = off - all;

        memset(sendpack.strmsg, 0, sizeof(sendpack.strmsg));
        len = read(fd, sendpack.strmsg, (size_t)need);
// for debug        
        // char * ptr = sendpack.strmsg;
        // printf("> len = %lld\n", len);
        // for (int j = 0; j < len; j++, ptr++)
        //     printf("%d -> %c\n", j, *ptr);
// end ---------
        sendpack.statusflag = len;
        if (len == -1)
        {
            printf("读取文件过程中出现问题!\n");
            perror("read");
            close(sock_fd);
            exit(1);
        }
        all += len;

        ret = SendMSG(sock_fd, &sendpack, PACK_SIZE, 0);
        if (ret < 0)
        {
            printf("服务器出错!\n");
            perror("SendMSG");
            close(sock_fd);
            exit(1);
        }        
    }

    // 发送结束包的接受
    sendpack.cmdflag = Flag_Cmd_SendFile;
    sendpack.source_id = user_id;
    sendpack.target_id = tarid;
    sendpack.statusflag = -111; 
    strcpy(sendpack.strmsg, filename);      // 存储文件名

    ret = SendMSG(sock_fd, &sendpack, PACK_SIZE, 0);
    if (ret < 0)
    {
        printf("服务器出错!\n");
        perror("SendMSG");
        close(sock_fd);
        exit(1);
    }       

// for debug --- 
    printf("all = %lld\toffset = %lld\n", all,off);
// end ---------

    printf("文件发送成功!\n");
    close(fd);
}

void FunGroupMenu(int grpid)
{
    int cmd, loop = 1;
    if (SearchMyGrp(grpid) == -1)
        printf("你未加入该群!\n");
    do
    {
        CLEAR(); 
        printf("                  - Function -                   \n\n");    
        printf("\t\t[1] 进入群聊\t\t\t\n");
        printf("\t\t[2] 查看聊天记录\t\t\t\n");
        printf("\t\t[3] 查看群成员\t\t\t\n");
        printf("\t\t[4] 退出群聊\t\t\t\n");
        printf("\t\t[5] 管理群[管理员]\t\t\t\n");
        printf("\t\t[6] 返回\t\t\t\n");

        printf("\n-------------------------------------------------\n");
        printf("\nYour Choice is :\t");
        scanf("%d", &cmd);
        while (getchar() != '\n');
        switch (cmd)
        {
            case 1:
                ChatWithWho = -1 * grpid;
                ChatWith(grpid, 0);
                ChatWithWho = 0;
                break;
            case 2:
                MessageHistory(grpid, 0);
                break;
            case 3:
                LookMember(grpid);
                PrintGrpMem();
                printf("按任意键退出\n");
                getchar();
                break;
            case 4:
                ExGroup(grpid, user_id);
                break;
            case 5:
                ConGroup(grpid);
                break;
            case 6:
                loop = 0;
                break;
            default:
                printf("无效命令\n");
        }
    }while (loop); 
}

// kind = 1, 私聊
// kind = 0, 群聊
void ChatWith(int tarid, int kind)
{
    CLEAR(); 
    int ret;
    char inputmsg[128], tempfile[30], msg[MSG_SIZE + 1];
    Package Msg;
    if (ChatWithWho < 0)
        sprintf(tempfile, "./%d", -1 * ChatWithWho);
    else
        sprintf(tempfile, "./%dand%d", user_id, ChatWithWho);

    int fd = open(tempfile, O_RDWR | O_CREAT | O_APPEND, S_IRWXU | S_IRGRP);
    if (fd == -1)
        perror("open");

    printf("                  - Chat with %d -                   \n\n", tarid);    
    while (s_gets(inputmsg, 510, stdin) && inputmsg[0] !='\0')
    {
        printf("\n");
        get_time(msg);
        strcat(msg, inputmsg);
        strcat(msg, "\n");
        write(fd, msg, strlen(msg));

        Msg.source_id = user_id;
        Msg.target_id = tarid;
        Msg.cmdflag = Flag_Cmd_Msg;
        sprintf(Msg.strmsg, "[%d] >> %s", user_id, inputmsg);

        if (kind)
            Msg.statusflag = MSG_FriNOR;
        else   
            Msg.statusflag = MSG_GRPNOR;

        ret = SendMSG(sock_fd, &Msg, PACK_SIZE, 0);
        if (ret < 0)
        {
            close(sock_fd);
            my_err(__FILE__, "SendMSG", __LINE__);
        }

        if (strcmp(inputmsg, "quit") == 0)
            break;
    }
    
    close(fd);
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
    {
        close(sock_fd);
        my_err(__FILE__, "SendMSG", __LINE__);
    }

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
    {
        close(sock_fd);
        my_err(__FILE__, "SendMSG", __LINE__);
    }

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

// kind == 1 私聊记录   kind == 0 群聊记录
void MessageHistory(int tarid, int kind)
{
    char tempfile[30], tempbuf[512];
    int tempid = kind == 0 ? -1 * tarid : tarid;
    if (kind == 1)
        sprintf(tempfile, "/%dand%d", user_id, tempid);
    else 
        sprintf(tempfile, "./%d", tarid);
    int fd = open(tempfile, O_RDONLY | O_CREAT, S_IRWXU);
    if (fd == -1)
        perror("open");
    printf("----------------------------------------\n\n");

    read(fd, tempbuf, sizeof(tempbuf));
    printf("%s", tempbuf);

    printf("\n------------------ end -----------------\n");

    close(fd);

    printf("按任意键退出\n");
    while (getchar() != '\n');
}

void DeleteSome(int tarid)
{
    int ret;
    if (tarid == user_id)
    {
        printf("错误的操作!\n");
        return ;
    }

    Package sendpack;
    sendpack.cmdflag = Flag_Cmd_DelFri;
    sendpack.source_id = user_id;
    sendpack.target_id = tarid;

    ret = SendMSG(sock_fd, &sendpack, PACK_SIZE, 0);
    if (ret < 0)
    {
        close(sock_fd);
        my_err(__FILE__, "SendMSG", __LINE__);
    }
    
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
    if (SearchMyFri(tarid) >= 0)
    {
        printf("他已经是你的好友了，请不要重复添加！\n");
        return ;
    }
    sendpack.cmdflag = Flag_Cmd_AddFri;
    sendpack.source_id = user_id;
    sendpack.target_id = tarid;
    
    ret = SendMSG(sock_fd, &sendpack, PACK_SIZE, 0);
    if (ret < 0)
    {
        close(sock_fd);
        my_err(__FILE__, "SendMSG", __LINE__);
    }

    printf("添加好友申请已发送\n");
    printf("按回车键退出\n");
    while (getchar() != '\n')
        ;
    return ;
}

void LookMsgBox()
{
    int choice, cmd, ret;

    Package sendpack;
    CLEAR();
    printf("                  - MessageBox -                   \n\n");

    while(1)
    {
        int num = MessageBox.size();
        for (int i = 0; i < num; i++)
        {
            printf("[%d]\t", i + 1);
            if (MessageBox[i].kind != MSG_FriNOR && MessageBox[i].kind != MSG_GRPNOR)
                printf("[System Message]\t");
            else    
                printf("[%s'%d'给你发来一条信息]\t", MessageBox[i].kind == MSG_FriNOR ? "用户" : "群聊", MessageBox[i].sourceid);
            printf("%s\n", MessageBox[i].msgtime);
            printf("\t%s\n", MessageBox[i].msg);
        }

        printf("-------------------------------------------\n");

        printf("\n请选择要处理的系统消息, 无效选择将会退出\n");
        printf("[Your Choice]\t");
        if (scanf("%d", &choice) != 1)
        {
            while (getchar() != '\n');
            return ;
        }
        while (getchar() != '\n');
        if (choice <= 0 || choice > num)
            return;
        if (MessageBox[choice - 1].kind == MSG_FriNOR)
        {
            ChatWithWho = MessageBox[choice - 1].sourceid;
            ChatWith(MessageBox[choice - 1].sourceid, 1);
            ChatWithWho = 0;
            MessageBox.erase(MessageBox.begin() + choice - 1);
        }
        else if (MessageBox[choice - 1].kind == MSG_GRPNOR)
        {
            ChatWithWho = -1 * MessageBox[choice - 1].sourceid;
            printf("%d: ChatWithWho = %d\n", __LINE__, ChatWithWho);
            ChatWith(MessageBox[choice - 1].sourceid, 0);
            ChatWithWho = 0;
            MessageBox.erase(MessageBox.begin() + choice - 1);
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
                    my_err(__FILE__, "SendMSG", __LINE__);
                printf("success\n");
            }
            MessageBox.erase(MessageBox.begin() + choice - 1);
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
                sendpack.target_id = MessageBox[choice - 1].targetid;

                printf("sendpack.cmdflag = %d\n", sendpack.cmdflag);
                printf("sendpack.source_id = %d\n", sendpack.source_id);
                printf("sendpack.target_id = %d\n", sendpack.target_id);
                printf("sendpack.strmsg = %s\n", sendpack.strmsg);

                ret = SendMSG(sock_fd, &sendpack, PACK_SIZE, 0);
                if (ret < 0)
                    my_err(__FILE__, "SendMSG", __LINE__);
            }
            MessageBox.erase(MessageBox.begin() + choice - 1);
        }
        else if (MessageBox[choice - 1].kind == MSG_SYS_RECVFILE)
        {
            printf("%s\n", MessageBox[choice - 1].msg);
            printf("是否接受文件？(1/0)\n");
            scanf("%d", &cmd);
            if (cmd)
            {
                sendpack.cmdflag = Flag_Cmd_RecvFile;   // 请求接受
                sendpack.source_id = user_id;
                sendpack.target_id = MessageBox[choice - 1].sourceid;

                ret = SendMSG(sock_fd, &sendpack, PACK_SIZE, 0);    
                if (ret < 0)
                    my_err(__FILE__, "SendMSG", __LINE__);
            }
            MessageBox.erase(MessageBox.begin() + choice - 1);
        }
    }
}

void Exit()
{
    int ret;
    Package sendpack;
    sendpack.cmdflag = Flag_Cmd_Exit;
    sendpack.source_id = user_id;

    ret = SendMSG(sock_fd, &sendpack, PACK_SIZE, 0);
    if (ret < 0)
    {
        close(sock_fd);
        my_err(__FILE__, "SendMSG", __LINE__);
    }
}

void LookMember(int grpid)
{
    int ret;
    Package sendpack;
    sendpack.source_id = user_id;
    sendpack.cmdflag = Flag_Cmd_LkGrpMem;
    sendpack.target_id = grpid;
    GrpMem.clear();             // 清除原有数据

    ret = SendMSG(sock_fd, &sendpack, PACK_SIZE, 0);
    if (ret < 0)
    {
        close(sock_fd);
        my_err(__FILE__, "SendMSG", __LINE__);
    }
    while (flag == 0)  
        ;
    flag = 0;
}

void GetMemberList(Package recvpack)
{
    // 解析
    char * str1, * str2;
    char tempid[12], tempsta[5];
    str1 = strstr(recvpack.strmsg, _END_);
    str2 = strstr(str1 + strlen(_END_), _END_);
    strncpy(tempid, recvpack.strmsg, str1 - recvpack.strmsg);
    tempid[str1 - recvpack.strmsg] = '\0';
    strncpy(tempsta, str1 + strlen(_END_), str2 - str1 - strlen(_END_));
    tempsta[str2 - str1 - strlen(_END_)] = '\0';

    sMember m;
    m.id = atoi(tempid);
    m.status = atoi(tempsta);

    GrpMem.push_back(m);                  // 放入vector
}

// 如果 user_id 不是管理员 还需要一个search函数
void ExGroup(int grpid, int user)
{
    int ret;
    int pos = SearchInGroup(user);
    if (pos < 0)
    {
        printf("你还不是群成员\n");
        return;
    }
    else if (GrpMem[pos].status == 2)
    {
        printf("你是群主，无法退出该群!\n");
        return;
    }
    Package sendpack;
    sendpack.cmdflag = Flag_Cmd_RemvSome;
    sendpack.source_id = grpid;
    sendpack.target_id = user;

    ret = SendMSG(sock_fd, &sendpack, PACK_SIZE, 0);
    if (ret < 0)
    {
        close(sock_fd);
        my_err(__FILE__, "SendMSG", __LINE__);
    }
}

// 查找群成员
int SearchInGroup(int userid)
{
    int num = GrpMem.size();
    for (int i = 0; i < num; i++)
    {
        if (GrpMem[i].id == userid)
            return i;
    }
    return -1;
}

void ConGroup(int grpid)
{
    int ret, loop = 1;
    int pos = SearchInGroup(user_id);
    if (pos < 0)
    {
        printf("你还不是群成员\n");
        return;
    }
    else if (GrpMem[pos].status == 0)
    {
        printf("你是普通群员，没有管理权限!\n");
        return;
    }
    do
    {
        CLEAR(); 
        printf("                  - Function -                   \n\n");    
        printf("\t\t[1] 将某人移出群\t\t\t\n");
        printf("\t\t[2] 解散群组[群主]\t\t\t\n");
        printf("\t\t[3] 设置管理员[群主]\t\t\t\n");
        printf("\t\t[4] 取消管理员[群主]\t\t\t\n");
        printf("\t\t[5] 返回\t\t\t\n");

        int cmd;
        if (scanf("%d", &cmd) != 1)
        {
            while (getchar() != '\n')
                ;
            continue;
        }
        switch (cmd)
        {
            case 1:
                RemoveSome(grpid);
                break;
            case 2:
                DisBand(grpid);
                break;
            case 3:
                SetConMem(grpid);
                break;
            case 4:
                UnSetConMem(grpid);
                break;
            case 5:
                loop = 0;
                break;
            default:
                printf("无效命令!\n");
                break;
        }
        LookMember(grpid);
    }while(loop);
}

void RemoveSome(int grpid)
{
    int one;
    CLEAR();
    printf("请输入要删除的群成员:\n");
    scanf("%d", &one);
    int pos = SearchInGroup(one);
    if (pos < 0)
    {
        printf("该成员不在本群!\n");
        return ;
    }
    else if (GrpMem[pos].status != 0)
    {
        printf("对方不是普通群员，你无权将他移出群聊!\n");
        return;
    }
    else 
    {
        ExGroup(grpid, one);
    }
}

void DisBand(int grpid)
{
    int ret;
    Package sendpack;
    int pos = SearchInGroup(user_id);
    if (pos < 0)
    {
        printf("你不是该群的成员！\n");
        return ;
    }
    else if (GrpMem[pos].status != 2)
    {
        printf("你不是群主，无权解散群!\n");
        return;
    }
    else 
    {
        sendpack.cmdflag = Flag_Cmd_DisBand;
        sendpack.source_id = user_id;
        sendpack.target_id = grpid;
        memset(sendpack.strmsg, 0, sizeof(sendpack.strmsg));

        ret = SendMSG(sock_fd, &sendpack, PACK_SIZE, 0);
        if (ret < 0)
        {
            close(sock_fd);
            my_err(__FILE__, "SendMSG", __LINE__);
        }
    }
}

void SetConMem(int grpid)
{
    int one, ret, pos;
    Package sendpack;

    pos = SearchInGroup(user_id);
    if (pos < 0)
    {
        printf("你不是该群的成员！\n");
        return ;
    }
    else if (GrpMem[pos].status != 2)
    {
        printf("你不是群主，无权设置管理员!\n");
        return ;
    }
    
    printf("请输入要设置管理员的用户id:\n");
    if (scanf("%d", &one) != 1)
    {
        while (getchar() != '\n')
            ;
        return ;
    }
    pos = SearchInGroup(one);
    if (pos < 0)
    {
        printf("对方不是群成员，无法操作\n");
        return;
    }
    
    sendpack.cmdflag = Flag_Cmd_SetCtrl;
    sendpack.source_id = grpid;
    sendpack.target_id = one;

    ret = SendMSG(sock_fd, &sendpack, PACK_SIZE, 0);
    if (ret < 0)
    {
        close(sock_fd);
        my_err(__FILE__, "SendMSG", __LINE__);
    }

    printf("设置对方为管理员成功!\n");

}

void UnSetConMem(int grpid)
{
    int one, ret, pos;
    Package sendpack;

    pos = SearchInGroup(user_id);
    if (pos < 0)
    {
        printf("你不是该群的成员！\n");
        return ;
    }
    else if (GrpMem[pos].status != 2)
    {
        printf("你不是群主，无权设置管理员!\n");
        return ;
    }
    
    printf("请输入要取消管理员的用户id:\n");
    if (scanf("%d", &one) != 1)
    {
        while (getchar() != '\n')
            ;
        return ;
    }
    pos = SearchInGroup(one);
    if (pos < 0)
    {
        printf("对方不是群成员，无法操作\n");
        return;
    }
    
    sendpack.cmdflag = Flag_Cmd_UnSetCtrl;
    sendpack.source_id = grpid;
    sendpack.target_id = one;

    ret = SendMSG(sock_fd, &sendpack, PACK_SIZE, 0);
    if (ret < 0)
    {
        close(sock_fd);
        my_err(__FILE__, "SendMSG", __LINE__);
    }

    printf("取消对方为管理员成功!\n");
}

void AddGrp(int id)
{
    int ret, pos;
    Package sendpack;

    if (SearchMyGrp(id) > 0)
    {
        printf("你已加入该群，无须重复添加\n");
        return ;
    }
    sendpack.cmdflag = Flag_Cmd_AddGrp;
    sendpack.source_id = user_id;
    sendpack.target_id = id;

    ret = SendMSG(sock_fd, &sendpack, PACK_SIZE, 0);
    if (ret < 0)
    {
        close(sock_fd);
        my_err(__FILE__, "SendMSG", __LINE__);
    }

    printf("添加群组申请已发送，请等待管理员审核\n");
    printf("按回车键退出\n");
    while (getchar() != '\n')
        ;
    return;
}

void CreateGrp(void)
{
    int ret;
    Package sendpack;
    char grpname[USER_NAME_MAX + 1], grpsome[OTHER_SIZE + 1];
    do
    {
        CLEAR(); 
        printf("*------------------- Chat Room -------------------*\n");
        printf("*                 - Create Group -                  *\n\n");
        printf("请输入群名：\n");
    }while (!s_gets(grpname, USER_NAME_MAX, stdin) || grpname[0] == '\0');
    printf("请输入群介绍:\n");
    s_gets(grpsome, OTHER_SIZE, stdin);

    sprintf(sendpack.strmsg, "%s%s%s%s", grpname, _END_, grpsome, _END_);
    sendpack.source_id = user_id;
    sendpack.cmdflag = Flag_Cmd_CreateGrp;

    ret = SendMSG(sock_fd, &sendpack, PACK_SIZE, 0);
    if (ret < 0)
    {
        close(sock_fd);
        my_err(__FILE__, "SendMSG", __LINE__);
    }
    sleep(1);
    printf("按回车键退出\n");
    while (getchar() != '\n')
                    ;
}

void PrintGrpMem(void)
{
    int num = (int)GrpMem.size();
    if (num == 0)
        printf("群成员列表为空\n");
    for (int i = 0; i < num; i++)
    {
        printf("[%d] ", GrpMem[i].id);
        if (GrpMem[i].status == 0)
            printf("普通群员\n");
        else if (GrpMem[i].status == 1)
            printf("管理员\n");
        else if (GrpMem[i].status == 2)
            printf("群主\n");
    }
    printf("\n");
}
