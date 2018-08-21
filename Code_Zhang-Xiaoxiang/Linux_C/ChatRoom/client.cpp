// ChatRoom_client
#include "base.h"
#include "view.h"
// #include "account.h"
#include "internet.h"

#define PATH "./.MSG"                   
// Directory: 存放聊天记录
// 目录下文件命名格式： PATH/[f/g]+[userid/groupid].txt
 
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;      // 新建一个锁，防止读写冲突

void Initializer(void);                 // 客户端登录后同步信息 好友列表、群列表、好友状态

void* Precv(void * arg);                // 线程: 接受消息

void* Pupdate(void * arg);              // 线程: 更新好友上下线状态、好友数量变动、群聊数量变动

int main(void)
{
    signal(SIGPIPE, SIG_IGN);           // 屏蔽sigpipe信号

    int ret;                            // 检测返回值
    int sock_fd;                        // 套接字描述符
    

    return 0;
}
