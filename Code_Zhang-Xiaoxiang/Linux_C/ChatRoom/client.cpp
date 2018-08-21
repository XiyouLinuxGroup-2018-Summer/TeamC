// ChatRoom_client
#include "base.h"
#include "view.h"
#include "internet.h"

using namespace std;

#define PATH "./.MSG"                   
// Directory: 存放聊天记录
// 目录下文件命名格式： PATH/[f/g]+[userid/groupid].txt
typedef struct fri
{
    int id;
    char name[USER_NAME_MAX + 1];
}sFriend;
typedef struct grp 
{
    int id;
    char name[USER_NAME_MAX + 1];
    char introduction[OTHER_SIZE + 1];
    int mem[MEM_NUM];
}sGroup;
sFriend myFriend[FRI_NUM];
sFriend myGroup[GRP_NUM];
int MsgNum = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;      // 新建一个锁，防止读写冲突

void Initializer(void);                                 // 客户端登录后同步信息 好友列表、群列表、好友状态

void* Precv(void * arg);                                // 线程: 接受消息

void* Pupdate(void * arg);                              // 线程: 更新好友上下线状态、好友数量变动、群聊数量变动

int main(void)
{
    signal(SIGPIPE, SIG_IGN);                           // 屏蔽sigpipe信号

    int ret, loopflag = 1, status = 0;                  // status: 登录状态 loopflag: 循环标识 ret: 函数返回值处理
    int user_id;                                        // 用户id
    int sock_fd;                                        // 套接字描述符
    char recvbuf[BUF_SIZE], sendbuf[BUF_SIZE];          // 设置缓冲区
    pthread_t Trecv, Tupdate;                           // 线程id
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
        my_err(__FILE__, "connect", __LINE__);

    // 开始界面
    while (loopflag)
    {
        Menu:ret = vBeginRLQ();
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

    Initializer();                                      // 初始化，从服务器获取一些信息, 更新消息
    loopflag = 1;
    ret = pthread_create(&Trecv, NULL, Precv, NULL);    // 创建线程接受服务器传来的消息
    if (ret < 0)
        my_err(__FILE__, "pthread_create", __LINE__);
    
    while (1)
    {
        vMessageTop(MsgNum);                            // 消息提示
        printf("\n\n\n");
        ret = vFunMainMenu();
        switch(ret)
        {
            case 1: // 查看离线消息

                pthread_mutex_lock(&mutex);
                MsgNum = 0;       
                pthread_mutex_unlock(&mutex);  
                // 消息数目置零
                break;
            case 2: // 查看好友列表
                // 选择好友进行操作
                break;
            case 3: // 查看群组列表
                break;
            case 4: // 添加好友
                break;
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
