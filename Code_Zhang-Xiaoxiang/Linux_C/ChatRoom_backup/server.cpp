#include "base.h"
#include "internet.h"
#include "sql.h"
#include <vector>
#include <set>
#include <map>
using namespace std;

#define LISTENUM            999     // 请求连接队列长度
#define EVENTS_MAX          1000    // epoll 监测的最多事件
#define DEBUG

static int sock_fd;
map<int, int>IdWithSock;            // user_id --> socket
map<int, int>SockWithId;            // socket  --> user_id
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;  // 初始化锁

MYSQL * _mysql = NULL;
int epfd, nfds;
struct epoll_event ev, events[EVENTS_MAX];
void Initia(void);                  // 初始化
void EndEvents(void);               // 关闭连接，套接字等等
void* pthreadFun(void * arg);       // 线程，处理请求

int main(void)
{
    signal(SIGPIPE, SIG_IGN);       // 屏蔽pipe信号, 防止客户端异常退出后服务端挂掉
    Initia();  
    int ret, op;
    int conn_fd;                    // 套接字
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

                get_time(NULL);
                fprintf(stdout, "[CMD] accept [IP] %s\n", inet_ntoa(cli_addr.sin_addr));

                ev.data.fd = conn_fd;
                ev.events = EPOLLIN;
                epoll_ctl(epfd, EPOLL_CTL_ADD, conn_fd, &ev);
            }
            else if (events[i].events & EPOLLIN)    // 收到package, 处理
            {
                conn_fd = events[i].data.fd;
                // if (pthread_create(&pth, NULL, pthreadFun, (void*)&conn_fd) < 0)
                //     my_err(__FILE__, "pthread_create", __LINE__, 0);
                pthreadFun((void*)&conn_fd);
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

void reset_oneshot(int epollfd, int fd)     // 重置事件
{
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
    epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &event);
}

void* pthreadFun(void * arg)
{
    int ret, kase, sourceid;
    int confd = *(int*)arg;
    map<int, int>::iterator it;     // 迭代器
    Package recvpack, sendpack;

    // 接收包
    ret = RecvMSG(confd, &recvpack, PACK_SIZE, 0);
    fprintf(stderr, "ret == %d \n", ret);
    if (ret < 0)
    {
        my_err(__FILE__, "RecvMSG", __LINE__, 1);
        close(confd);
        return 0;
        // pthread_exit(0);
    } 
    else if (ret == 0)                          // 客户度异常退出　[软件中断]
    {
        epoll_ctl(epfd, EPOLL_CTL_DEL, confd, &ev);
        printf("=======================\n");
        int temp_userid;
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
        // pthread_exit(0);
        return 0;
    }

    kase = recvpack.cmdflag;
    sourceid = recvpack.source_id;
    sendpack = recvpack;

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
            close(confd);
            fprintf(stdout, "[status] 客户端正常退出 [socket] %d\n\n", confd);
            // pthread_exit(0);
            return 0;
        }
        case Flag_Cmd_Msg:
        {
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
                    GrpMemberList(grp_id, mem, NULL); // 获取群成员列表
                    for (int i = 0; mem[i] != 0; i++)
                    {
                        recvpack.source_id = grp_id;
                        if (mem[i] == sourceid)             // 跳过自己
                            continue;
                        it = IdWithSock.find(mem[i]);
                        if (it == IdWithSock.end())                 // 目的用户处于离线状态
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
                            ret = SendMSG(tar_socket, &recvpack, PACK_SIZE, 0);
                            if (ret < 0)
                                my_err(__FILE__, "SendMSG", __LINE__, 1);
                        }
                    }

                    break;
                }
                case MSG_FriNOR:
                {
                    int taruser_id = recvpack.target_id, tar_socket;
                    fprintf(stdout, "[id] `%d` 给 [id] `%d`　发送了一条私聊信息\n", sourceid, taruser_id);
                    it = IdWithSock.find(taruser_id);
                    if (it == IdWithSock.end())                 // 目的用户处于离线状态
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
                        ret = SendMSG(tar_socket, &recvpack, PACK_SIZE, 0);
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
            char temp[128];
            if (SearchAccId(sourceid, temp) == NULL)      // 如果目标对象不存在直接退出
                break;

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

            it = IdWithSock.find(temp_id);
            if (it == IdWithSock.end())                             // 如果用户未登录
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

            if (ret)                                                // 如果登录成功记录id和socket的对应关系
            {
                IdWithSock[temp_id] = confd;
                SockWithId[confd]   = temp_id;
            }

            sendpack.statusflag = ret;
            ret = SendMSG(confd, &sendpack, PACK_SIZE, 0);          // 给客户端发送回执
            if (ret < 0)
                my_err(__FILE__, "SendMSG", __LINE__, 1);
            
            break;
        }
        case Flag_Cmd_Register:
        {
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
            answer[str4-str3 - strlen(_END_)] = '\0';

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
                my_err(__FILE__, "SendMSG", __LINE__, 1);

            break;
        }
        case Flag_Cmd_AddFri:                   // 添加好友请求
        {
            int tar_id = recvpack.target_id;            // 要加的对象
            sendpack.cmdflag = Flag_Cmd_Msg;
            sendpack.statusflag = MSG_SYS_INVIFri;
            char temp[128];
            if (SearchAccId(tar_id, temp) == NULL)      // 如果目标对象不存在直接退出
                break;

            sprintf(sendpack.strmsg, "[System] %d 请求添加你为好友", sourceid);

            it = IdWithSock.find(tar_id);
            if (it == IdWithSock.end())                 // 如果对方不在线
            {
                ret = OfflineMSG(&sendpack, tar_id, MSG_SYS_INVIFri);
                if (ret < 0)
                    my_err(__FILE__, "OfflineMSG", __LINE__, 1);
            }
            else
            {
                int tar_socket = IdWithSock[tar_id];
                ret = SendMSG(tar_socket, &sendpack, PACK_SIZE, 0);
                if (ret < 0)
                    my_err(__FILE__, "SendMSG", __LINE__, 1);
            }
            break;
        }
        case Flag_Cmd_DelFri:
        {
            int tar_id = recvpack.target_id; 
            ret = DelFriend(tar_id, sourceid);
            get_time(NULL);
            if (ret == 0)
                fprintf(stderr, "删除好友过程中发生了一些问题。\n");
            break;
        }
        case Flag_Cmd_LkFriList:
        {
            fprintf(stdout, "接收到查询用户关系请求\n");
            int count = 0;
            char temp[FRI_NUM][50];
            memset(temp, 0, sizeof(temp));
            printf("source id = %d\n", sourceid);
            count = UserRelaList(sourceid, temp, 0);

            printf("count = %d\n", count);
            sendpack.statusflag = count;        // 第一次传输数据包的数目
            fprintf(stdout, "将会发送 %d 个包\n", count + 1);
            memset(sendpack.strmsg, 0, sizeof(sendpack.strmsg));
            ret = SendMSG(confd, &sendpack, PACK_SIZE, 0);
            if (ret < 0)
                my_err(__FILE__, "SendMSG", __LINE__, 1);

            fprintf(stdout, "发送第一次包成功\n");
            for (int i = 0; i < count; i++)
            {
                printf("temp[count] = %s", temp[i]);
                sendpack.cmdflag = Flag_Cmd_LkFriList;
                sendpack.source_id = sourceid;
                strcpy(sendpack.strmsg, temp[i]);
                ret = SendMSG(confd, &sendpack, PACK_SIZE, 0);
                if (ret < 0)
                    my_err(__FILE__, "SendMSG", __LINE__, 1);
                printf("==> 发送包结束\n");
            }
            fprintf(stdout, "发送结束\n");
            break;
        }
        case Flag_Cmd_LkGrpList:
        {
            int count = 0;
            char temp[FRI_NUM][50];
            memset(temp, 0, sizeof(temp));
            count = UserRelaList(sourceid, temp, 1);

            sendpack.statusflag = count;        // 第一次传输数据包的数目
            memset(sendpack.strmsg, 0, sizeof(sendpack.strmsg));
            ret = SendMSG(confd, &sendpack, PACK_SIZE, 0);
            if (ret < 0)
                my_err(__FILE__, "SendMSG", __LINE__, 1);

            for (int i = 0; i < count; i++)
            {
                printf("temp[count] = %s", temp[i]);
                strcpy(sendpack.strmsg, temp[i]);
                ret = SendMSG(confd, &sendpack, PACK_SIZE, 0);
                if (ret < 0)
                    my_err(__FILE__, "SendMSG", __LINE__, 1);
            }
            break;
        }
        case Flag_Cmd_SerIfInGrp:
        {
            int tar_id = recvpack.target_id;
            ret = SearchOneInGrp(tar_id, sourceid);
            sendpack.statusflag = ret;
            ret = SendMSG(confd, &sendpack, PACK_SIZE, 0);
            if (ret < 0)
                my_err(__FILE__, "SendMSG", __LINE__, 0);
            break;
        }
        case Flag_Cmd_ShiSome:
        {
            int tar_id = recvpack.target_id;
            ShiFriend(sourceid, tar_id);
            break;
        }
        case Flag_Cmd_UnShiSome:
        {
            int tar_id = recvpack.target_id;
            UnShiFriend(sourceid, tar_id);
            break;
        }
        case Flag_Cmd_CreateGrp:
        {
            // name + end + something + end
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
                my_err(__FILE__, "SendMSG", __LINE__, 1);
            break;
        }
        case Flag_Cmd_AddGrp:
        {
            int tar_id = recvpack.target_id, box[MEM_NUM + 1], temp, tar_socket, count = 0;
            memset(box, 0, sizeof(box));
            sendpack.cmdflag = Flag_Cmd_Msg;
            sendpack.statusflag = MSG_SYS_AGRAGRP;  // 修改包为一个申请加群请求的消息包
            sprintf(sendpack.strmsg, "[System] %d 请求加入群 %d", sourceid, tar_id);
            
            SearchStaGrp(tar_id, GRP_STA_CON, box + 1);
            SearchStaGrp(tar_id, GRP_STA_OWN, box);
            
            while (box[count] != 0)
            {
                sendpack.cmdflag = Flag_Cmd_Msg;
                sendpack.statusflag = MSG_SYS_AGRAGRP;  // 修改包为一
                temp = box[count];
                it = IdWithSock.find(temp);
                if (it == IdWithSock.end())     // 如果不在线
                {
                    fprintf(stdout, "目标用户[%d]处于离线状态，私聊信息将存入数据库中\n", temp);
                    ret = OfflineMSG(&sendpack, temp, MSG_SYS_AGRAGRP);
                    if (ret == 0)
                        fprintf(stderr, "[ERROR] 离线消息存储失败\n");
                }
                else
                {
                    fprintf(stdout, "目标用户[%d]在线\n", temp);
                    tar_socket = IdWithSock[temp];        // 目标套接字
                    ret = SendMSG(tar_socket, &sendpack, PACK_SIZE, 0);
                    if (ret < 0)
                        my_err(__FILE__, "SendMSG", __LINE__, 1);
                }
                count++;
            }
            break;
        }
        case Flag_Cmd_SyncOffMsg:
        {
            char offmsg[OffMsg_NUM][256];
            memset(offmsg, 0, sizeof(offmsg));
            // 查询sourceid的离线记录
            int num = TransOffMsg(sourceid, offmsg);
            sendpack.statusflag = num;
            ret = SendMSG(confd, &sendpack, PACK_SIZE, 0);
            if (ret < 0)
                    my_err(__FILE__, "SendMSG", __LINE__, 0);                
            for (int i = 0; i < num; i++)
            {
                strcpy(sendpack.strmsg, offmsg[i]);
                printf("%s", sendpack.strmsg);
                ret = SendMSG(confd, &sendpack, PACK_SIZE, 0);
                if (ret < 0)
                    my_err(__FILE__, "SendMSG", __LINE__, 0);
            }
            break;
        }
        case Flag_Cmd_LkGrpMem:     // 查看群成员
        {
            int grp_id = recvpack.target_id;
            int memid[MEM_NUM], mem_sta[MEM_NUM];
            memset(memid, 0, sizeof(memid));
            memset(memid, 0, sizeof(memid));
            memset(memid, 0, sizeof(memid));

            int num = GrpMemberList(grp_id, memid, mem_sta);

            sendpack.cmdflag = Flag_Cmd_LkGrpMem;
            sendpack.source_id = sourceid;
            sendpack.statusflag = num;
            sendpack.target_id = grp_id;

            ret = SendMSG(confd, &sendpack, PACK_SIZE, 0);
            if (ret < 0)
                my_err(__FILE__, "SendMSG", __LINE__, 0);
            printf("获取第一次数据包成功, 还要发送%d个包\n", num);
            
            for (int i = 0; i < num; i++)
            {
                char temp[MSG_SIZE];
                sprintf(temp, "%d%s%d%s", memid[i], _END_, mem_sta[i], _END_);
                printf("temp: %s\n", temp);
                sendpack.cmdflag = Flag_Cmd_LkGrpMem;
                sendpack.source_id = sourceid;
                sendpack.statusflag = num;
                sendpack.target_id = grp_id;
                strcpy(sendpack.strmsg, temp);

                ret = SendMSG(confd, &sendpack, PACK_SIZE, 0);
                if (ret < 0)
                    my_err(__FILE__, "SendMSG", __LINE__, 0);
                printf("%d > 获取包成功\n", i + 1);
            }

            break;
        }
        case Flag_Cmd_DisBand:      // 解散群
        {
            int tarid = recvpack.target_id;
            DelGroup(tarid);
            break;
        }
        case Flag_Cmd_SetCtrl:      // 设置管理员
        {
            SetCtrl(recvpack.source_id, recvpack.target_id);
            break;
        }
        case Flag_Cmd_UnSetCtrl:    // 取消设置管理员
        {
            UnSetCtrl(recvpack.source_id, recvpack.target_id);
            break;
        }
        case Flag_Cmd_RemvSome:     // 将某人移出群
        {
            int sor = recvpack.source_id;   // 群id
            int tar = recvpack.target_id;   // 用户id
            ReOneFromGrp(tar, sor);
            break;
        }
        case Flag_Cmd_SendFile:     // 发文件
        {
            int fd, ret;
            long long len, all;
            char filename[256], * str;
            sprintf(filename, "./.file/%d_%d", recvpack.source_id, recvpack.target_id);

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
                // for debug --- 
                char tempcmd[300];
                sprintf(tempcmd, "md5sum %s", filename);
                system(tempcmd);
                // end ---------

                // 给用户发送有文件到来的信息
                sendpack.cmdflag = Flag_Cmd_SendFile;       // 有用户给发来了一个文件
                sendpack.source_id = recvpack.source_id;    // 文件的发送者
                sendpack.target_id = recvpack.target_id;
                sprintf(sendpack.strmsg , "[system] %d 给你发送了一个文件(%s)", recvpack.source_id, recvpack.strmsg);
                it = IdWithSock.find(sendpack.target_id);   // 查找状态
                if (it == IdWithSock.end())                 // 离线
                {
                    fprintf(stdout, "目标用户[%d]处于离线状态 - 信息将存入数据库中\n", sendpack.target_id);
                    ret = OfflineMSG(&sendpack, sendpack.target_id, MSG_SYS_RECVFILE);
                }
                else                                        // 在线
                {
                    fprintf(stdout, "目标用户[%d]在线\n", sendpack.target_id);
                    
                    int tar_socket = IdWithSock[sendpack.target_id];
                    ret = SendMSG(tar_socket, &sendpack, PACK_SIZE, 0);
                    if (ret < 0)
                        my_err(__FILE__, "SendMSG", __LINE__, 1);
                }
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
#ifdef DEBUG
                char * ptr = recvpack.strmsg;
                // for (int j = 0; j < all; j++, ptr++)
                //     printf("%d -> %c\n", j, *ptr);
                printf(">> 接收到一个包\n");
#endif
                close(fd);
            }

            break;
        }
        case Flag_Cmd_RecvFile:     // 收文件
        {
            Package sendpack;
            char filename[256];
            int len;
            int tarid = recvpack.source_id;        // 要发送到的用户端
            int tar_socket = IdWithSock[tarid];    // 对端的套接字
            int sourc = recvpack.target_id;        // 文件来源

            sprintf(filename, "./server%d", sourc);
            int fd = open(filename, O_RDONLY);
            int off = lseek(fd, 0, SEEK_END);
            int num = off / MSG_SIZE + 1;           // 要发送包的数量
            if (off % MSG_SIZE == 0)
                num --;
            lseek(fd, 0, SEEK_SET);                 // 重置文件指针

            for (int i = 0; i < num; i++)
            {
                sendpack.cmdflag = Flag_Cmd_RecvFile;
                sendpack.source_id = sourc;
                sendpack.target_id = tarid;

                len = read(fd, sendpack.strmsg, sizeof(sendpack.strmsg));
                if (len == -1)
                {
                    printf("读取文件过程中出现问题!\n");
                    perror("read");
                    close(tar_socket);
                    return 0;
                }
                sendpack.statusflag = len;

                ret = SendMSG(tar_socket, &sendpack, PACK_SIZE, 0);
                if (ret < 0)
                {
                    perror("SendMSG");
                    close(tar_socket);
                    return 0;
                }     
            }

            sendpack.cmdflag = Flag_Cmd_RecvFile;
            sendpack.source_id = sourc;
            sendpack.target_id = tarid;
            sendpack.statusflag = -111;

            ret = SendMSG(tar_socket, &sendpack, PACK_SIZE, 0);
            if (ret < 0)
            {
                perror("SendMSG");
                close(tar_socket);
                return 0;
            }     
            break;
        }
        case Flag_Cmd_InvAddFri:    // 同意添加好友请求
        {
            // username + end + tarname + end
            char * str1, * str2;
            char username[USER_NAME_MAX + 1], tarname[USER_NAME_MAX + 1];
            int tar_id = recvpack.target_id;

            SearchAccId(sourceid, username);
            SearchAccId(tar_id, tarname);
            printf("username = %s\n", username);
            printf("tarname = %s\n", tarname);

            ret = AddFriend(sourceid, tar_id, username, tarname);

            if (ret == 0)
            {
                get_time(NULL);
                fprintf(stderr, "同意添加好友失败");
            }
            printf("添加好友成功\n");

            break;
        }
        case Flag_Cmd_InvAddMem:    // 同意加群请求
        {

            int grp_id = recvpack.target_id;
            char username[USER_NAME_MAX + 1], grpname[USER_NAME_MAX + 1];
            char * str1, * str2;
            
            SearchAccId(sourceid, username);
            SearchGrpId(grp_id, grpname);

            ret = AddOneToGrp(sourceid, username, grp_id, grpname);
            if (ret == 0)
            {
                get_time(NULL);
                fprintf(stderr, "同意加群失败");
            }
            break;
        }
        default:
        {
            get_time(NULL);
            fprintf(stderr, "[Warnning] 接收到的数据包异常\n\n");
            // pthread_exit(0);
            break;
        }
    }
    reset_oneshot(epfd, confd);
    // pthread_exit(0);
    return 0;
}
