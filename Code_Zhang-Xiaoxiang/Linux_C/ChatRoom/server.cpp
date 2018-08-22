#include "base.h"
#include "internet.h"
#include "sql.h"
#include <vector>
#include <set>
#include <map>
using namespace std;

#define LISTENUM            999     // 请求连接队列长度
#define EVENTS_MAX          1000    // epoll 监测的最多事件

#define MSG_FriNOR          0       // 私聊信息
#define MSG_GRPNOR          1       // 群聊信息
#define MSG_SYS_RECVFILE    2       // 系统信息: 接受文件
#define MSG_SYS_INVIGRP     3       // 系统信息: 好友邀群
#define MSG_SYS_INVIFri     4       // 系统信息: 加好友请求
#define MSG_SYS_AGRAGRP     5       // 系统信息: 同意加好友请求

static int sock_fd;
map<int, in_addr>SockWithIp;        // socket --> IP
map<int, int>IdWithSock;            // user_id --> socket
map<int, int>SockWithId;            // socket  --> user_id
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;  // 初始化锁

void Initia(void);                  // 初始化
void EndEvents(void);               // 关闭连接，套接字等等
void* pthreadFun(void * arg);       // 线程，处理请求

int main(void)
{
    signal(SIGPIPE, SIG_IGN);       // 屏蔽pipe信号, 防止客户端异常退出后服务端挂掉
    int ret, op;
    int conn_fd;           // 套接字
    int epfd, nfds;
    struct epoll_event ev, events[EVENTS_MAX];
    struct sockaddr_in ser_addr, cli_addr;
    pthread_t pth;                  // 线程id
    socklen_t cli_len = sizeof(struct sockaddr_in);

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0)
        my_err(__FILE__, "sock", __LINE__, 0);
    op = 1;
    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEPORT, (void*)&op, sizeof(int)) < 0)
        my_err(__FILE__, "setsockopt", __LINE__, 0);

    // 初始化服务器端地址结构
    memset(&ser_addr, 0, sizeof(struct sockaddr_in));
    ser_addr.sin_port = htons(PORT);
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // 将套接字绑定在本地端口
    ret = bind(sock_fd, (struct sockaddr *)&ser_addr, sizeof(struct sockaddr_in));
    if (ret < 0)
        my_err(__FILE__, "bind", __LINE__, 0);

    // 将套接字转化为监听套接字
    ret = listen(sock_fd, LISTENUM);
    if (ret < 0)
        my_err(__FILE__, "listen", __LINE__, 0);

    Initia();                       
    // mysql 初始化，用户、群列表，群成员信息, 用户状态 几个stl初始化, 清空

    epfd = epoll_create(EVENTS_MAX - 5);           
    ev.data.fd = sock_fd;
    ev.events = EPOLLIN;
    epoll_ctl(epfd, EPOLL_CTL_ADD, sock_fd, &ev);

    while (1)
    {
        nfds = epoll_wait(epfd, events, EVENTS_MAX, -1);

        for (int i = 0; i < nfds; i++)
        {
            if (events[i].data.fd == sock_fd)       // accept
            {
                conn_fd = accept(sock_fd, (struct sockaddr*)&cli_addr, &cli_len);
                if (conn_fd < 0)
                    my_err(__FILE__, "accept", __LINE__, 0);
                
                // 数组方式可以覆盖之前的值
                SockWithIp[conn_fd] = cli_addr.sin_addr;

                get_time(NULL);
                fprintf(stdout, "[CMD] accept [IP] %s [PORT] %s", inet_ntoa(cli_addr.sin_addr), cli_addr.sin_port);

                ev.data.fd = conn_fd;
                ev.events = EPOLLIN;
                epoll_ctl(epfd, EPOLL_CTL_ADD, conn_fd, &ev);
            }
            else if (events[i].events & EPOLLIN)    // 收到package, 处理
            {
                conn_fd = events[i].data.fd;
                if (pthread_create(&pth, NULL, pthreadFun, (void*)&conn_fd) < 0)
                    my_err(__FILE__, "pthread_create", __LINE__, 0);
            }
        }
    }

    EndEvents();
    return 0;
}

void Initia(void)
{
    // 初始化mysql和连接
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
    SockWithIp.clear();
    IdWithSock.clear();
}

void EndEvents(void)
{
    mysql_close(_mysql);
    close(sock_fd);
    pthread_mutex_destroy(&mutex);
    SockWithIp.clear();
    IdWithSock.clear();
}

void* pthreadFun(void * arg)
{
    int ret, kase, sourceid;
    int confd = *(int*)arg;
    map<int, int>::iterator it;     // 迭代器
    Package recvpack, sendpack;
    memset(&sendpack, 0, sizeof(Package));

    // 接收包
    ret = RecvMSG(confd, &recvpack, PACK_SIZE, 0);
    if (ret < 0)
        my_err(__FILE__, "RecvMSG", __LINE__, 1);
    else if (ret == 0)                          // 客户度异常退出　[软件中断]
    {
        int temp_userid;
        it = SockWithId.find(confd);
        get_time(NULL);
        if (it != SockWithId.end())             // 如果confd对应的操作者是登录用户
        {
            temp_userid = it->second;
            SockWithId.erase(it);               // 从记录登录的两个map中删除
            IdWithSock.erase(temp_userid);
            printf("[user](id = %d) exit\n", temp_userid);
        }
        fprintf(stdout, "[status] 客户端退出（软件中断）[IP] %s [socket] %s\n", inet_ntoa(SockWithIp[confd]), confd);
        pthread_exit(0);
    }

    kase = recvpack.cmdflag;
    sourceid = recvpack.source_id;

    // 执行相关操作
    switch(kase)
    {
        case Flag_Cmd_Exit:     
        {
            int temp_userid;
            it = SockWithId.find(confd);
            get_time(NULL);
            if (it != SockWithId.end())             // 如果confd对应的操作者是登录用户
            {
                temp_userid = it->second;
                SockWithId.erase(it);               // 从记录登录的两个map中删除
                IdWithSock.erase(temp_userid);
                printf("[user](id = %d) exit\n", temp_userid);
            }
            fprintf(stdout, "[status] 客户端正常退出 [IP] %s [socket] %s\n\n", inet_ntoa(SockWithIp[confd]), confd);
            pthread_exit(0);
            break;
        }
        // case Flag_Cmd_Empty:
        //     break;
        case Flag_Cmd_Msg:
        {
            sendpack = recvpack;
            get_time(NULL);
            fprintf(stdout, "服务器接收到聊天请求:\n");

            switch (recvpack.statusflag)
            {
                case Flag_Status_Group:
                {
                    int grp_id = recvpack.target_id, tar_socket;
                    fprintf(stdout, "[id] `%d` 在群　[id] `%d`　发送了一条群聊信息\n", sourceid, grp_id);
                    // 应该查找一下这个群还在不在
                    int mem[MEM_NUM] = {0};       
                    GrpMemberList(grp_id, mem, NULL, NULL); // 获取群成员列表
                    for (int i = 0; mem[i] != 0; i++)
                    {
                        if (mem[i] == sourceid)         // 跳过自己
                            continue;
                        it = SockWithId.find(mem[i]);
                        if (it == SockWithId.end())                 // 目的用户处于离线状态
                        {
                            fprintf(stdout, "目标用户[%d]处于离线状态，私聊信息将存入数据库中\n", mem[i]);
                            ret = OfflineMSG(&recvpack, mem[i], MSG_GRPNOR);
                            if (ret == 0)
                                fprintf(stderr, "[ERROR] 离线消息存储失败\n");
                        }
                        else
                        {
                            fprintf(stdout, "目标用户[%d]在线\n", mem[i]);
                            tar_socket = IdWithSock[mem[i]];        // 目标套接字
                            ret = SendMSG(tar_socket, &sendpack, PACK_SIZE, 0);
                            if (ret < 0)
                                my_err(__FILE__, "SendMSG", __LINE__, 1);
                        }
                        break;
                    }

                    break;
                }
                case Flag_Status_user:
                {
                    int taruser_id = recvpack.target_id, tar_socket;
                    fprintf(stdout, "[id] `%d` 给 [id] `%d`　发送了一条私聊信息\n", sourceid, taruser_id);
                    it = SockWithId.find(taruser_id);
                    if (it == SockWithId.end())                 // 目的用户处于离线状态
                    {
                        fprintf(stdout, "目标用户处于离线状态，私聊信息将存入数据库中\n");
                        ret = OfflineMSG(&recvpack, taruser_id, MSG_FriNOR);
                        if (ret == 0)
                            fprintf(stderr, "[ERROR] 离线消息存储失败\n");
                    }
                    else
                    {
                        fprintf(stdout, "目标用户在线\n");
                        tar_socket = IdWithSock[taruser_id];        // 目标套接字
                        ret = SendMSG(tar_socket, &sendpack, PACK_SIZE, 0);
                        if (ret < 0)
                            my_err(__FILE__, "SendMSG", __LINE__, 1);
                    }
                    break;
                }
            }
            break;
        }
        case Flag_Cmd_Login:
        {
            get_time(NULL);
            fprintf(stdout, "接收到登录请求\n");
            int temp_id;
            char temp_strid[12], temp_pass[USER_PASS_MAX], * pstr = recvpack.strmsg, * pstr2;
            while (strncmp(pstr, _END_, 3) != 0)
                pstr++;
            strncpy(temp_strid, recvpack.strmsg, pstr - recvpack.strmsg);
            temp_id = atoi(temp_strid);
            pstr2 = pstr + 3;
            while (strncmp(pstr2, _END_, 3) != 0)
                pstr2++;
            *pstr2 = '\0';
            strcpy(temp_pass, pstr + 3);
            ret = LoginAcc(temp_id, temp_pass);
            fprintf(stdout, "[%d] 尝试登录%s\n", temp_id, ret ? "成功" : "失败");

            sendpack.cmdflag = ret;
            ret = SendMSG(confd, &sendpack, PACK_SIZE, 0);          // 给客户端发送回执
            if (ret < 0)
                my_err(__FILE__, "SendMSG", __LINE__, 1);
            
            break;
        }
        case Flag_Cmd_Register:
        {
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
            answer[str4-str3 - strlen(_END_)] = '\0';
        }
        // case 4:
        //     break;
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
        case 8:
        {
            break;
        }
        case 9:
        {
            break;
        }
        case 10:
        {
            break;
        }
        case 11:
        {
            break;
        }
        case 12:
        {
            break;
        }
        case 13:
        {
            break;
        }
        case 14:
        {
            break;
        }
        case 15:
        {
            break;
        }
        case 16:
        {
            break;
        }
        case 17:
        {
            break;
        }
        case 18:
        {
            break;
        }
        case 19:
        {
            break;
        }
        case 20:
        {
            break;
        }
        case 21:
        {
            break;
        }
        case 22:
        {
            break;
        }
        case 23:
        {
            break;
        }
        default:
        {
            get_time(NULL);
            fprintf(stderr, "[Warnning] 接收到的客户端数据包异常\n\n");
            pthread_exit(0);
            break;
        }
    }
    pthread_exit(0);

}
