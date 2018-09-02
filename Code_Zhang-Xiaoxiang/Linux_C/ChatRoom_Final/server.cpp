/* ChatRoom - server */
#include <vector>
#include <set>
#include <map>
#include "base.h"
#include "internet.h"
#include "sql.h"
using namespace std;

#define LISTENUM            999                     // 请求连接队列长度
#define EVENTS_MAX          1000                    // epoll 监测的最多事件
#define DEBUG                                       // for DEBUG

static int sock_fd;
static map<int, int>IdWithSock;                            // user_id --> socket
static map<int, int>SockWithId;                            // socket  --> user_id
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;  // 初始化锁
MYSQL * _mysql = NULL;                              // Mysql 句柄
static int epfd, nfds;                              // epoll
static struct epoll_event ev, events[EVENTS_MAX];

void Initia(void);                                  // 初始化
void EndEvents(void);                               // 清理操作
void ExecCmd(int confd);                            // 根据包的类型执行先相关操作
void reset_oneshot(int epollfd, int fd);            // 重置事件
int FindOnline(int id);                             // 查找某人是否在线
void Exec_Login(Package recvpack, int confd);       // 处理登录
void Exec_Msg(Package recvpack, int confd);         // 处理消息
void Exec_Resigster(Package recvpack, int confd);   // 处理注册
void Exec_AddFri(Package recvpack, int confd);      // 添加好友
void Exec_DelFri(Package recvpack, int confd);      // 删除好友
void Exec_FriList(Package recvpack, int confd);     // 好友列表
void Exec_GrpList(Package recvpack, int confd);
void Exec_GrpMemList(Package recvpack, int confd);
void Exec_ShiSome(Package recvpack);
void Exec_UnShi(Package recvpack);
void Exec_CreateGrp(Package recvpack, int confd);
void Exec_AddGrp(Package recvpack, int confd);
void Exec_SyncOffMsg(Package recvpack, int confd);
void Exec_DisBand(Package recvpack);
void Exec_SetCtrl(Package recvpack);
void Exec_UnSetC(Package recvpack);
void Exec_RemoveOne(Package recvpack);
void Exec_SendFile(Package recvpack, int confd);
void Exec_RecvFile(Package recvpack, int confd);
void Exec_InvAddFri(Package recvpack);
void Exec_InvAddMem(Package recvpack);

int main(void)
{
    signal(SIGPIPE, SIG_IGN);                       // 屏蔽pipe信号
    Initia();                                       // 初始化

    int ret, op;
    int conn_fd;  
    struct sockaddr_in ser_addr, cli_addr;
    socklen_t cli_len = sizeof(struct sockaddr_in);

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0)
        my_err(__FILE__, "sock", __LINE__);
    op = 1;
    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEPORT, (void*)&op, sizeof(int)) < 0)
        my_err(__FILE__, "setsockopt", __LINE__);

    // 初始化服务器端地址结构
    memset(&ser_addr, 0, sizeof(struct sockaddr_in));
    ser_addr.sin_port = htons(PORT);
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // 将套接字绑定在本地端口
    ret = bind(sock_fd, (struct sockaddr *)&ser_addr, sizeof(struct sockaddr_in));
    if (ret < 0)
        my_err(__FILE__, "bind", __LINE__);

    // 将套接字转化为监听套接字
    ret = listen(sock_fd, LISTENUM);
    if (ret < 0)
        my_err(__FILE__, "listen", __LINE__);
                     
    epfd = epoll_create(EVENTS_MAX - 5);           
    ev.data.fd = sock_fd;
    ev.events = EPOLLIN;
    epoll_ctl(epfd, EPOLL_CTL_ADD, sock_fd, &ev);

    while (1)
    {
        nfds = epoll_wait(epfd, events, EVENTS_MAX, -1);

        for (int i = 0; i < nfds; i++)
        {
            if (events[i].data.fd == sock_fd)       // 收到 accept 请求
            {
                conn_fd = accept(sock_fd, (struct sockaddr*)&cli_addr, &cli_len);
                if (conn_fd < 0)
                    my_err(__FILE__, "accept", __LINE__);

                get_time(NULL);
                fprintf(stdout, "[CMD] accept [IP] %s\n", inet_ntoa(cli_addr.sin_addr));

                ev.data.fd = conn_fd;
                ev.events = EPOLLIN;
                epoll_ctl(epfd, EPOLL_CTL_ADD, conn_fd, &ev);       // 将对方的套接字加入监听队列
            }
            else if (events[i].events & EPOLLIN)                    // else 必须，否则两个if条件可能会同时满足
            {
                conn_fd = events[i].data.fd;
                ExecCmd(conn_fd);
            }
        }
    }

    EndEvents();
    return 0;
}

void Initia(void)
{
    _mysql = mysql_init(_mysql);
    if (_mysql == NULL)
    {
        fprintf(stderr, "mysql init failed.\n");
        exit(1);
    }
    if ((mysql_real_connect(_mysql, NULL, MS_USER, MS_PASS, Data_ChatBase, 0, NULL, 0)) == NULL)
    {
        fprintf(stderr, "%d: %s\n", __LINE__, mysql_error(_mysql));
        exit(1);
    }
    // 初始化 stl
    SockWithId.clear();
    IdWithSock.clear();
}

void EndEvents(void)
{
    mysql_close(_mysql);
    close(sock_fd);
    pthread_mutex_destroy(&mutex);
    SockWithId.clear();
    IdWithSock.clear();
}

void reset_oneshot(int epollfd, int fd)                         
{
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
    epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &event);
}

int FindOnline(int _id)
{
    map<int, int>::iterator it;     // 迭代器
    it = IdWithSock.find(_id);      // 查找套接字
    if (it == IdWithSock.end()) 
        return -1;
    return IdWithSock[_id];
}

void ExecCmd(int confd)
{
    int ret, kase, temp_userid, sourceid;
    Package recvpack;
    map<int, int>::iterator it;

    // 接受包
    ret = RecvMSG(confd, &recvpack, PACK_SIZE, 0);
    if (ret < 0)
    {
        my_err(__FILE__, "RecvMSG", __LINE__);
        close(confd);
        return ;
    }
    else if (ret == 0 || recvpack.cmdflag == Flag_Cmd_Exit)
    {
        epoll_ctl(epfd, EPOLL_CTL_DEL, confd, &ev);
        it = SockWithId.find(confd);
        get_time(NULL);
        if (it != SockWithId.end())             // 如果confd对应的操作者是登录用户
        {
            temp_userid = it->second;
            SockWithId.erase(it);               // 从记录登录的两个map中删除
            IdWithSock.erase(temp_userid);
            fprintf(stdout, "[%d] exit\n", temp_userid);
        }
        fprintf(stdout, "[status] 客户端退出（软件中断）[socket] %d\n", confd);
        close(confd);
        return ;
    }

    kase = recvpack.cmdflag;
    sourceid = recvpack.source_id;

    // 解析包
    switch (kase)
    {
        case Flag_Cmd_Msg:              // 消息
            Exec_Msg(recvpack, confd);
            break;
        case Flag_Cmd_Login:            // 登录
            Exec_Login(recvpack, confd);
            break;
        case Flag_Cmd_Register:         // 注册
            Exec_Resigster(recvpack, confd);
            break;
        case Flag_Cmd_AddFri:           // 添加好友
            Exec_AddFri(recvpack, confd);
            break;
        case Flag_Cmd_DelFri:           // 删除好友
            Exec_DelFri(recvpack, confd);
            break;
        case Flag_Cmd_LkFriList:        // 查看好友列表
            Exec_FriList(recvpack, confd);
            break;
        case Flag_Cmd_LkGrpList:        // 查看群组列表
            Exec_GrpList(recvpack, confd);
            break;
        case Flag_Cmd_ShiSome:          // 屏蔽某人
            Exec_ShiSome(recvpack);
            break;
        case Flag_Cmd_UnShiSome:        // 解除对某人的屏蔽
            Exec_UnShi(recvpack);
            break;
        case Flag_Cmd_CreateGrp:        // 创建群
            Exec_CreateGrp(recvpack, confd);
            break;
        case Flag_Cmd_AddGrp:           // 添加群
            Exec_AddGrp(recvpack, confd);
            break;
        case Flag_Cmd_SyncOffMsg:       // 同步离线消息
            Exec_SyncOffMsg(recvpack, confd);
            break;
        case Flag_Cmd_LkGrpMem:         // 查看群成员
            Exec_GrpMemList(recvpack, confd);
            break;
        case Flag_Cmd_DisBand:          // 解散群组
            Exec_DisBand(recvpack);
            break;
        case Flag_Cmd_SetCtrl:          // 设置管理员
            Exec_SetCtrl(recvpack);
            break;
        case Flag_Cmd_UnSetCtrl:        // 取消设置管理员
            Exec_UnSetC(recvpack);
            break;
        case Flag_Cmd_RemvSome:         // 将某人移出群
            Exec_RemoveOne(recvpack);
            break;
        case Flag_Cmd_SendFile:         // 发送文件
            Exec_SendFile(recvpack, confd);
            break;
        case Flag_Cmd_RecvFile:         // 接受文件
            Exec_RecvFile(recvpack, confd);
            break;
        case Flag_Cmd_InvAddFri:        // 同意添加好友
            Exec_InvAddFri(recvpack);
            break;
        case Flag_Cmd_InvAddMem:        // 同意加群请求
            Exec_InvAddMem(recvpack);
            break;
        default:
        {
            get_time(NULL);
            fprintf(stderr, "[Warnning] 接收到的数据包异常\n\n");
            break;
        }
    }
    reset_oneshot(epfd, confd);
    return ;
}

void Exec_Msg(Package recvpack, int confd)
{
    int ret, sourceid = recvpack.source_id;

    get_time(NULL);
    fprintf(stdout, "服务器接收到聊天请求:\n"); 

    switch (recvpack.statusflag)
    {
        case MSG_GRPNOR:
        {
            int grp_id = recvpack.target_id, tar_socket;

            fprintf(stdout, "[id] `%d` 在群　[id] `%d`　发送了一条群聊信息\n", sourceid, grp_id);
            // 应该查找一下这个群还在不在
            int mem[MEM_NUM] = {0};       
            GrpMemberList(grp_id, mem, NULL);       // 获取群成员列表

            for (int i = 0; mem[i] != 0; i++)
            {
                recvpack.source_id = grp_id;
                if (mem[i] == sourceid)             // 跳过自己
                    continue;
                tar_socket = FindOnline(mem[i]);
                if (tar_socket < 0)
                {
                    fprintf(stdout, "目标用户[%d]处于离线状态，私聊信息将存入数据库中\n", mem[i]);
                    ret = OfflineMSG(&recvpack, mem[i], MSG_GRPNOR);
                    if (ret == 0)
                        fprintf(stderr, "[ERROR] 离线消息存储失败\n");
                }
                else
                {
                    fprintf(stdout, "目标用户[%d]在线\n", mem[i]);
                    ret = SendMSG(tar_socket, &recvpack, PACK_SIZE, 0);
                    if (ret < 0)
                        my_err(__FILE__, "SendMSG", __LINE__);
                }
            }
            break;
        }
        case MSG_FriNOR:
        {
            int taruser_id = recvpack.target_id, tar_socket;
            fprintf(stdout, "[id] `%d` 给 [id] `%d`　发送了一条私聊信息\n", sourceid, taruser_id);
            
            if (IsShield(taruser_id, sourceid) == 1)
            {
                fprintf(stdout, "发送方已被接收方屏蔽\n");
                break;
            }

            tar_socket = FindOnline(taruser_id);
            if (tar_socket < 0)                             // 目的用户处于离线状态
            {
                fprintf(stdout, "目标用户处于离线状态，私聊信息将存入数据库中\n");
                ret = OfflineMSG(&recvpack, taruser_id, MSG_FriNOR);
                if (ret == 0)
                    fprintf(stderr, "[ERROR] 离线消息存储失败\n");
            }
            else
            {
                fprintf(stdout, "目标用户在线\n");
                ret = SendMSG(tar_socket, &recvpack, PACK_SIZE, 0);
                if (ret < 0)
                    my_err(__FILE__, "SendMSG", __LINE__);
            }
            break;
        }
    }
}   

void Exec_Login(Package recvpack, int confd)
{
    int sourceid = recvpack.source_id, temp_id, tarsocket, ret;
    char temp[128], temp_strid[12], temp_pass[USER_PASS_MAX], * pstr = recvpack.strmsg, * pstr2;
    Package sendpack = recvpack;

    get_time(NULL);
    fprintf(stdout, "接收到登录请求\n");
    if (!SearchAccId(sourceid, temp))      // 如果目标对象不存在直接退出
    {
        strcpy(sendpack.strmsg, "账号不存在!请重新再试.\n");
        sendpack.statusflag = 0;
    }
    else
    {
        while (strncmp(pstr, _END_, 3) != 0)
                pstr++;
        strncpy(temp_strid, recvpack.strmsg, pstr - recvpack.strmsg);
        temp_id = atoi(temp_strid);
        pstr2 = pstr + 3;
        while (strncmp(pstr2, _END_, 3) != 0)
            pstr2++;
        *pstr2 = '\0';
        strcpy(temp_pass, pstr + 3);

        tarsocket = FindOnline(temp_id);
        if (tarsocket == -1)                                    // 如果用户未登录
        {
            ret = LoginAcc(temp_id, temp_pass);
            fprintf(stdout, "[%d] 尝试登录%s\n", temp_id, ret ? "成功" : "失败");
            if (ret == 0)
                strcpy(sendpack.strmsg, "账号不存在或密码错误!请重新再试.\n");
        }   
        else                                                    // 如果账号已被登录，拒绝请求
        {
            ret = 0;
            strcpy(sendpack.strmsg, "该账号已登录，请勿重复登录\n");
        }

        if (ret)                                                // 如果登录成功记录
        {
            IdWithSock[temp_id] = confd;
            SockWithId[confd]   = temp_id;
        }
        sendpack.statusflag = ret;
    }

    ret = SendMSG(confd, &sendpack, PACK_SIZE, 0);          // 给客户端发送回执
    if (ret < 0)
        my_err(__FILE__, "SendMSG", __LINE__);  
}

void Exec_Resigster(Package recvpack, int confd)
{
    Package sendpack = recvpack;
    int ret, sourceid = recvpack.source_id;

    get_time(NULL);
    fprintf(stdout, "接收到注册请求\n");

    char * str1, * str2, *str3, *str4;
    char username[USER_NAME_MAX + 1], pass[USER_PASS_MAX + 1], question[OTHER_SIZE + 1], answer[OTHER_SIZE + 1];
    str1 = strstr(recvpack.strmsg, _END_);
    str2 = strstr(str1 + strlen(_END_), _END_);
    str3 = strstr(str2 + strlen(_END_), _END_);
    str4 = strstr(str3 + strlen(_END_), _END_);
    strncpy(username, recvpack.strmsg, str1 - recvpack.strmsg);
    username[str1 - recvpack.strmsg] = '\0';
    strncpy(pass, str1 + strlen(_END_), str2 - str1 - strlen(_END_));
    pass[str2 - str1 - strlen(_END_)] = '\0';
    strncpy(question, str2 + strlen(_END_), str3 - str2 - strlen(_END_));
    question[str3 - str2 - strlen(_END_)] = '\0';
    strncpy(answer, str3 + strlen(_END_), str4 - str3 - strlen(_END_));
    answer[str4 - str3 - strlen(_END_)] = '\0';

    ret = RegACC(username, pass, question, answer);
    fprintf(stdout, "[%d] 注册%s\n", sourceid, ret ? "成功" : "失败");

    if (ret)
    {
        IdWithSock[ret]   = confd;
        SockWithId[confd] = ret; 
    }

    sendpack.statusflag = ret;
    ret = SendMSG(confd, &sendpack, PACK_SIZE, 0);
    if (ret < 0)
        my_err(__FILE__, "SendMSG", __LINE__);

}

void Exec_AddFri(Package recvpack, int confd)
{
    int sourceid = recvpack.source_id, ret, tarsocket;
    Package sendpack = recvpack;

    int tar_id = recvpack.target_id;            // 要加的对象
    sendpack.cmdflag = Flag_Cmd_Msg;
    sendpack.statusflag = MSG_SYS_INVIFri;
    char temp[128];
    if (!SearchAccId(tar_id, temp))      // 如果目标对象不存在直接退出
        return ;

    sprintf(sendpack.strmsg, "[System] %d 请求添加你为好友", sourceid);

    tarsocket = FindOnline(tar_id);
    if (tarsocket == -1)                        // 如果对方不在线
    {
        ret = OfflineMSG(&sendpack, tar_id, MSG_SYS_INVIFri);
        if (ret < 0)
            my_err(__FILE__, "OfflineMSG", __LINE__);
    }
    else
    {
        ret = SendMSG(tarsocket, &sendpack, PACK_SIZE, 0);
        if (ret < 0)
            my_err(__FILE__, "SendMSG", __LINE__);
    }
}

void Exec_DelFri(Package recvpack, int confd)
{
    int ret, sourceid = recvpack.source_id;

    int tar_id = recvpack.target_id; 
    ret = DelFriend(tar_id, sourceid);
    get_time(NULL);
    if (ret == 0)
        fprintf(stderr, "删除好友过程中发生了一些问题。\n");
}

void Exec_FriList(Package recvpack, int confd)
{
    int sourceid = recvpack.source_id;
    int ret, count = 0, fri_id;
    char temp[FRI_NUM][50];
    Package sendpack = recvpack;

    fprintf(stdout, "接收到查询用户关系请求\n");
    memset(temp, 0, sizeof(temp));
    printf("source id = %d\n", sourceid);
    count = UserRelaList(sourceid, temp, 0);

    printf("count = %d\n", count);
    fprintf(stdout, "将会发送 %d 个包\n", count);

    for (int i = 0; i < count; i++)
    {
        printf("temp[count] = %s", temp[i]);
        sendpack.cmdflag = Flag_Cmd_LkFriList;
        sendpack.source_id = sourceid;
        sendpack.statusflag = 1;
    
        // 插入好友状态
        char * tempstr, id[11];
        tempstr = strstr(temp[i], _END_);
        strncpy(id, temp[i], (size_t)(tempstr - temp[i]));
        fri_id = atoi(id);
        ///////
        printf("friend id = %d\n", fri_id);
        ///////
        strcat(temp[i], FindOnline(fri_id) == -1 ? "0" : "1");
        strcat(temp[i], _END_);
        // 在线为1，离线为0
 
        strcpy(sendpack.strmsg, temp[i]);
        ret = SendMSG(confd, &sendpack, PACK_SIZE, 0);
        if (ret < 0)
            my_err(__FILE__, "SendMSG", __LINE__);
        printf("==> 发送了一个包\n");
    }
    
    // 标志发送包的结束
    sendpack.cmdflag = Flag_Cmd_LkFriList;
    sendpack.source_id = sourceid;
    sendpack.statusflag = -111;                     

    ret = SendMSG(confd, &sendpack, PACK_SIZE, 0);
    if (ret < 0)
            my_err(__FILE__, "SendMSG", __LINE__);
    
    fprintf(stdout, "发送结束\n");
}

void Exec_GrpList(Package recvpack, int confd)
{
    int count = 0, sourceid = recvpack.source_id, ret;
    char temp[FRI_NUM][50];
    Package sendpack = recvpack;

    memset(temp, 0, sizeof(temp));
    count = UserRelaList(sourceid, temp, 1);

    for (int i = 0; i < count; i++)
    {        
        printf("temp[count] = %s", temp[i]);
        sendpack.cmdflag = Flag_Cmd_LkGrpList;
        sendpack.statusflag = 1;

        strcpy(sendpack.strmsg, temp[i]);
        ret = SendMSG(confd, &sendpack, PACK_SIZE, 0);
        if (ret < 0)
            my_err(__FILE__, "SendMSG", __LINE__);
    }

    sendpack.cmdflag = Flag_Cmd_LkGrpList;
    sendpack.statusflag = -111;
    
    ret = SendMSG(confd, &sendpack, PACK_SIZE, 0);
    if (ret < 0)
        my_err(__FILE__, "SendMSG", __LINE__);
}

void Exec_ShiSome(Package recvpack)
{
    int tar_id = recvpack.target_id, sourceid = recvpack.source_id;
    ShiFriend(sourceid, tar_id);
}

void Exec_UnShi(Package recvpack)
{
    int tar_id = recvpack.target_id, sourceid = recvpack.source_id;
    UnShiFriend(sourceid, tar_id);
}

void Exec_CreateGrp(Package recvpack, int confd)
{
    int ret, sourceid = recvpack.source_id;
    Package sendpack;

    char * str1, * str2;
    char grpname[USER_NAME_MAX + 1], something[OTHER_SIZE + 1];
    str1 = strstr(recvpack.strmsg, _END_);
    str2 = strstr(str1 + strlen(_END_), _END_);
    strncpy(grpname, recvpack.strmsg, str1 - recvpack.strmsg);
    grpname[str1 - recvpack.strmsg] = '\0';
    strncpy(something, str1 + strlen(_END_), str2 - str1 - strlen(_END_));
    something[str2 - str1 - strlen(_END_)] = '\0';

    ret = CreateGroup(sourceid, grpname, something);
    sendpack.cmdflag = Flag_Cmd_CreateGrp;
    sendpack.statusflag = ret;

    ret = SendMSG(confd, &sendpack, PACK_SIZE, 0);
    if (ret < 0)            
        my_err(__FILE__, "SendMSG", __LINE__);    
}

void Exec_AddGrp(Package recvpack, int confd)
{
    int tar_id = recvpack.target_id, box[MEM_NUM + 1];
    int count = 0, ret, sourceid = recvpack.source_id;

    Package sendpack = recvpack;


    memset(box, 0, sizeof(box));
    sendpack.cmdflag = Flag_Cmd_Msg;
    sendpack.statusflag = MSG_SYS_AGRAGRP;      // 修改包为一个申请加群请求的消息包
    sprintf(sendpack.strmsg, "[System] %d 请求加入群 %d", sourceid, tar_id);
    
    SearchStaGrp(tar_id, GRP_STA_CON, box + 1);
    SearchStaGrp(tar_id, GRP_STA_OWN, box);
    
    while (box[count] != 0)
    {
        sendpack.cmdflag = Flag_Cmd_Msg;
        sendpack.statusflag = MSG_SYS_AGRAGRP;  // 修改包为一
        int temp = box[count];
        int tar_socket= FindOnline(temp);
        if (tar_socket == -1)             // 如果不在线
        {
            fprintf(stdout, "目标用户[%d]处于离线状态，私聊信息将存入数据库中\n", temp);
            ret = OfflineMSG(&sendpack, temp, MSG_SYS_AGRAGRP);
            if (ret == 0)
                fprintf(stderr, "[ERROR] 离线消息存储失败\n");
        }
        else
        {
            fprintf(stdout, "目标用户[%d]在线\n", temp);
            ret = SendMSG(tar_socket, &sendpack, PACK_SIZE, 0);
            if (ret < 0)
                my_err(__FILE__, "SendMSG", __LINE__);
        }
        count++;
    }    
}

void Exec_SyncOffMsg(Package recvpack, int confd)
{
    int sourceid = recvpack.source_id, ret;
    Package sendpack = recvpack;
    char offmsg[OffMsg_NUM][256];

    memset(offmsg, 0, sizeof(offmsg));
    // 查询sourceid的离线记录
    int num = TransOffMsg(sourceid, offmsg);
    sendpack.statusflag = num;
    ret = SendMSG(confd, &sendpack, PACK_SIZE, 0);
    if (ret < 0)
            my_err(__FILE__, "SendMSG", __LINE__);                
    for (int i = 0; i < num; i++)
    {
        strcpy(sendpack.strmsg, offmsg[i]);
        printf("%s", sendpack.strmsg);
        ret = SendMSG(confd, &sendpack, PACK_SIZE, 0);
        if (ret < 0)
            my_err(__FILE__, "SendMSG", __LINE__);
    }
}

void Exec_GrpMemList(Package recvpack, int confd)
{
    int ret, sourceid = recvpack.source_id;
    int memid[MEM_NUM], mem_sta[MEM_NUM];
    int grp_id = recvpack.target_id;
    Package sendpack = recvpack;

    memset(memid, 0, sizeof(memid));
    memset(memid, 0, sizeof(memid));
    memset(memid, 0, sizeof(memid));

    int num = GrpMemberList(grp_id, memid, mem_sta);
    printf("获取第一次数据包成功, 要发送%d个包\n", num);
    
    for (int i = 0; i < num; i++)
    {
        char temp[MSG_SIZE];
        sprintf(temp, "%d%s%d%s", memid[i], _END_, mem_sta[i], _END_);
        printf("temp: %s\n", temp);
        sendpack.cmdflag = Flag_Cmd_LkGrpMem;
        sendpack.source_id = sourceid;
        sendpack.statusflag = i;
        sendpack.target_id = grp_id;
        strcpy(sendpack.strmsg, temp);

        ret = SendMSG(confd, &sendpack, PACK_SIZE, 0);
        if (ret < 0)
            my_err(__FILE__, "SendMSG", __LINE__);
        printf("%d > 获取包成功\n", i + 1);
    }
    // 发送包表示数据结束
    sendpack.cmdflag = Flag_Cmd_LkGrpMem;
    sendpack.source_id = sourceid;
    sendpack.statusflag = -111;
    sendpack.target_id = grp_id;    

    ret = SendMSG(confd, &sendpack, PACK_SIZE, 0);
    if (ret < 0)
        my_err(__FILE__, "SendMSG", __LINE__);
}

void Exec_DisBand(Package recvpack)
{
    int tarid = recvpack.target_id;
    DelGroup(tarid);
}

void Exec_SetCtrl(Package recvpack)
{
    SetCtrl(recvpack.source_id, recvpack.target_id);    
}

void Exec_UnSetC(Package recvpack)
{
    UnSetCtrl(recvpack.source_id, recvpack.target_id);
}

void Exec_RemoveOne(Package recvpack)
{
    int sor = recvpack.source_id;   // 群id
    int tar = recvpack.target_id;   // 用户id
    ReOneFromGrp(tar, sor);    
}

void Exec_SendFile(Package recvpack, int confd)
{
    int fd, ret;
    long long len, all;
    char filename[256], * str;
    Package sendpack = recvpack;

    sprintf(filename, "./.file/%d_%d", recvpack.source_id, recvpack.target_id);

    if (recvpack.statusflag == -222)
    {
        fd = creat(filename, S_IRWXU);          // 创建文件
        if (fd == -1)
        {
            perror("create file failed");
            return;
        }
        close(fd);
    }
    else if (recvpack.statusflag == -111)
    {
        printf("文件接收完成!\n");
        // for debug --- 
        char tempcmd[300];
        sprintf(tempcmd, "md5sum %s", filename);
        system(tempcmd);
        // end ---------

        // 给用户发送有文件到来的信息
        sendpack.cmdflag = Flag_Cmd_SendFile;       
        sendpack.source_id = recvpack.source_id;    // 文件的发送者
        sendpack.target_id = recvpack.target_id;
        sprintf(sendpack.strmsg , "[system] %d 给你发送了一个文件", recvpack.source_id);

        int tar_socket = FindOnline(sendpack.target_id);
        if (tar_socket == -1)                 // 离线
        {
            fprintf(stdout, "目标用户[%d]处于离线状态 - 信息将存入数据库中\n", sendpack.target_id);
            ret = OfflineMSG(&sendpack, sendpack.target_id, MSG_SYS_RECVFILE);
        }
        else                                        // 在线
        {
            fprintf(stdout, "目标用户[%d]在线\n", sendpack.target_id);
            
            ret = SendMSG(tar_socket, &sendpack, PACK_SIZE, 0);
            if (ret < 0)
                my_err(__FILE__, "SendMSG", __LINE__);
        }
    }
    else 
    {
        fd = open(filename, O_WRONLY | O_APPEND);     // 追加
        if (fd == -1)
        {
            perror("open");
            return;
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
        // char * ptr = recvpack.strmsg;
        // for (int j = 0; j < all; j++, ptr++)
        //     printf("%d -> %c\n", j, *ptr);
        printf(">> 接收到一个包\n");
        close(fd);
    }
}

void Exec_RecvFile(Package recvpack, int confd)
{
    int ret, fd, off, all;
    int sourceid = recvpack.target_id, targetid = recvpack.source_id;
    Package sendpack = recvpack;
    char filename[256];

    sprintf(filename, "./.file/%d_%d", sourceid, targetid);
    fd = open(filename, O_RDONLY);
    if (fd == -1)
    {
        printf("文件打开失败！请检查路径是否正确和文件权限!\n");
        perror("open");
        return;        
    }

    off = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);     

    sendpack.cmdflag = Flag_Cmd_RecvFile;
    sendpack.statusflag = -222;
    sendpack.source_id = sourceid;
    sendpack.target_id = targetid;
    ret = SendMSG(confd, &sendpack, PACK_SIZE, 0);
    if (ret < 0)
    {
        my_err(__FILE__, "SendMSG", __LINE__);
        return;
    }

    all = 0;
    while (all < off)
    {
        sendpack.cmdflag = Flag_Cmd_RecvFile;
        sendpack.source_id = sourceid;
        sendpack.target_id = targetid;        

        int need, len;
        if (off - all > sizeof(sendpack.strmsg))
            need = sizeof(sendpack.strmsg);
        else 
            need = off - all;    

        memset(sendpack.strmsg, 0, sizeof(sendpack.strmsg));
        len = read(fd, sendpack.strmsg, (size_t)need);   

        sendpack.statusflag = len;
        if (len == -1)
        {
            printf("读取文件过程中出现问题!\n");
            perror("read");
            return ;
        }
        all += len;

        ret = SendMSG(confd, &sendpack, PACK_SIZE, 0);
        if (ret < 0)
        {
            printf("服务器出错!\n");
            perror("SendMSG");
            return;
        }        
    }

    sendpack.cmdflag = Flag_Cmd_RecvFile;
    sendpack.source_id = sourceid;
    sendpack.target_id = targetid;
    sendpack.statusflag = -111; 

    ret = SendMSG(confd, &sendpack, PACK_SIZE, 0);
    if (ret < 0)
    {
        printf("服务器出错!\n");
        perror("SendMSG");
        close(confd);
        return;
    }       

    printf("文件发送成功!\n");
}

// 添加好友之后通知双方
void Exec_InvAddFri(Package recvpack)
{
    int ret, tar_id, sourceid;
    Package sendpack = recvpack;
    char * str1, * str2;
    char username[USER_NAME_MAX + 1], tarname[USER_NAME_MAX + 1];
    tar_id = recvpack.target_id;
    sourceid = recvpack.source_id;
    
    if (!SearchAccId(sourceid, username))
    {
        printf("目标不存在!\n");
        return ;
    }
    if (!SearchAccId(tar_id, tarname))
    {
        printf("目标不存在!\n");
        return ;
    }
    printf("username = %s\n", username);
    printf("tarname = %s\n", tarname);

    ret = AddFriend(sourceid, tar_id, username, tarname);

    if (ret == 0)
    {
        get_time(NULL);
        fprintf(stderr, "同意添加好友失败");
    }
    printf("添加好友成功\n");
}

// 添加群之后通知对方
void Exec_InvAddMem(Package recvpack)
{
    int ret, sourceid = recvpack.source_id;
    int grp_id = recvpack.target_id;
    char username[USER_NAME_MAX + 1], grpname[USER_NAME_MAX + 1];
    
    if (!SearchAccId(sourceid, username))
    {
        printf("查找目标不存在!\n");
        return ;
    }
    if (!SearchGrpId(grp_id, grpname))
    {
        printf("查找目标不存在!\n");
        return ;        
    }

    ret = AddOneToGrp(sourceid, username, grp_id, grpname);
    if (ret == 0)
    {
        get_time(NULL);
        fprintf(stderr, "同意加群失败");
    }
}
