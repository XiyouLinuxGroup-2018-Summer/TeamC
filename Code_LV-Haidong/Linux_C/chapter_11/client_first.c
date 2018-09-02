#include "my_chattingroom.h"
#include "menu.h"
#include "main_menu.h"
#include "friend_menu.h"
#include "group_menu.h"
#include <mysql/mysql.h>
#include <unistd.h>
#include <unistd.h>
#include <signal.h>
#include "my_send_recv.h"

void client_login(int conn_fd);
void regist(int conn_fd);
void my_friend();
void *chatting_intput(void *arg);
void *chatting_output(void *arg);
void important_menu(qq information, int conn_fd);
void view_myfriend(qq information, int conn_fd);
void add_myfriend(qq information, int conn_fd);
void view_list(qq information, int conn_fd);
void delete_myfriend(qq information, int conn_fd);
void view_black(qq information, int conn_fd);
void my_create_group(qq information, int conn_fd);
void my_group_list(qq information, int conn_fd);   //获取我的群列表
void my_delete_group(qq information, int conn_fd); //解散群
void member_list(qq information, int conn_fd);     //查看群成员
void set_guanli(qq information, int conn_fd);      //设置管理员
void inivt_merber(qq information, int conn_fd);    //邀请成员进群
void chatting_group(qq information, int conn_fd);  //一对多的聊天
void kick_out_members(qq information, int conn_fd);
void init_file(qq information, int conn_fd);  //找到当前的好友
void send_file(qq information2, int conn_fd); //发送文件
void recv_file(qq information, int conn_fd);  //接受好友的文件
int my_news_box(qq information, int conn_fd); //我的消息盒子
void get_friend_news(qq information);
void get_file_one(qq information, int conn_fd);
void get_chatting_group(qq information);
void my_onlie_news(qq information, int conn_fd);
void view_news(qq information, int conn_fd);
void group_news_list(qq information, int conn_fd);
void my_group(qq information, int conn_fd);
void my_enter_group(qq information, int conn_fd);
int my_recv(void);

int my_connfd;
char group_num[13]; //群号
qq *head;           //消息盒子的头指针
char to[13];        ///表示目前是不是与你在聊天，判断当前用户是不是
int good;

qq *init()
{
    head = (qq *)malloc(sizeof(qq));
    head->next = NULL;
    return head;
}

//如果有一些信息的话，进入消息盒子进行处理

//显示好友的聊天记录
void view_news(qq information, int conn_fd)
{
    information.flag = 19;
    printf("请输入你想要查询消息的好友\n");
    scanf("%s", information.to);
    getchar();
    send(conn_fd, &information, sizeof(qq), 0);
}

void group_news_list(qq information, int conn_fd)
{
    information.flag = 20;
    printf("请输入你想要查询的群消息(输入quit取消查询)\n");
    scanf("%s", information.to);
    if (strcmp(information.to, "quit") == 0)
    {
        return;
    }
    send(conn_fd, &information, sizeof(qq), 0);
}

//如果有一些信息的话，进入消息盒子进行处理
void add_news_box(qq information2) // 加入消息盒子的函数
{
    qq *news;
    news = (qq *)malloc(sizeof(qq));
    news->flag = information2.flag;
    news->state = information2.state;
    strcpy(news->number, information2.number);
    strcpy(news->username, information2.username);
    strcpy(news->passwd, information2.passwd);
    strcpy(news->record, information2.record);
    strcpy(news->to, information2.to);
    news->next = head->next;
    head->next = news;
}

void recv_file(qq information, int conn_fd)
{

    char file_name[100];
    int fd;
    printf("passwd %s \n", information.passwd);
    printf("record %s \n", information.record);
    sprintf(file_name, "/home/kiosk/%s_%s.txt", information.to, information.passwd);
    fd = open(file_name, O_RDWR | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd < 0)
        my_err("此文件无法打开 ", __LINE__);
    write(fd, information.record, information.size);
    close(fd);
}

void get_file_one(qq information, int conn_fd)
{
    if (strcmp(information.to, to) == 0) //当前有没有处于聊天之中
    {

        printf("\t 好友%s 给你发来文件%s\n", information.to, information.record);
    }
    else
    {
        printf("\t好友%s发来一个文件,请到消息盒子处理\n", information.to);
        add_news_box(information);
        //不是当前聊天的群就放进消息盒子
    }
}

void send_file(qq information2, int conn_fd)
{
    information2.flag = 17;
    printf("name %s\n", information2.number);
    printf("to %s \n", information2.to);
    printf("flag %d\n", information2.flag);
    char name[256];
    int file_fd;
    int j, i;
    printf("请输入你想要发送文件的绝对路径\n");
    scanf("%s", name);
    getchar();
    printf("name = %s\n", name);
    file_fd = open(name, O_RDONLY);
    printf("file_fd %d\n", file_fd);
    if (file_fd < 0)
    {
        printf("此文件不存在;\n");
    }
    ///解析出文件名
    for (i = 0, j = 0; j < strlen(name); i++)
    {
        if (name[i] == '/')
        {
            j = 0;
            continue;
        }
        information2.passwd[j++] = name[i];
    }
    information2.passwd[j] = '\0';

    ///在这里采用的是定长的方式，其中sum表示是不是已经发完,和file_len比较
    int sum = 0, file_len = 0;
    int sum_length = 0; //文件的长度

    lseek(file_fd, 0, SEEK_SET); //转移到文件的首部

    sum_length = lseek(file_fd, 0, SEEK_END); //转移到尾部，获得此文件的大小

    lseek(file_fd, 0, SEEK_SET);
    printf("文件开始传输  > > > > >\n");

    while (sum != sum_length)
    {
        file_len = read(file_fd, information2.record, 120);
        // printf("%s\n",information2.record);
        if (file_len == -1)
        {
            my_err("read", __LINE__); //获得报错
        }
        sum += file_len;
        information2.size = file_len;
        // printf("information2.record %s \n",information2.record);
        // printf("send 前\n");
        send(my_connfd, &information2, sizeof(qq), 0);
        printf("client sum %d , client sum_length %d \n", sum, sum_length);
        // usleep(200);
    }
    printf("文件传输结束 < < <  <\n");
    getchar();
    // send(my_connfd,&information2,sizeof(qq),0);
}

int getch1() //已经封装好的getch函数
{
    int c = 0;
    int res = -1;
    struct termios org_opts, new_opts;

    res = tcgetattr(STDIN_FILENO, &org_opts);
    assert(res == 0);
    memcpy(&new_opts, &org_opts, sizeof(org_opts));
    new_opts.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | ECHOPRT | ECHOKE);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_opts);
    c = getchar();
    res = tcsetattr(STDIN_FILENO, TCSANOW, &org_opts);
    assert(res == 0);

    return c;
}

qq *user_freind() //将每个账号的好友用链表连接加数据库存储
{
    qq *temp;
    temp = (qq *)malloc(sizeof(qq));
    temp->next = NULL;
    return temp;
}

qq *init_news()
{
    qq *news_temp;
    news_temp = (qq *)malloc(sizeof(qq));
    news_temp->next = NULL;
    return news_temp;
}

//注册函数
void regist(int conn_fd)
{
    printf("\033c");
    int i = 0;
    char ch;
    char result[200];
    qq registername;
    struct termios old;
    struct termios news;
    printf("\t\t\t注册界面\t\t\t\n");
    printf("------------------------------------------------\n");
    printf("请输入账号(不能含有空格,下划线,不得少于八位)\n");
    scanf("%s", registername.number);
    printf("请输入名称\n");
    scanf("%s", registername.username);
    //old保存当前的终端参数，new用来保存修改后的终>
    memset(result, 0, 200);
    tcgetattr(0, &old);           //将tcgetattr()获取到的stdin参数存
    tcsetattr(0, TCSANOW, &news); //将修改后的new设置为stdin的新的参数,并且立即
    printf("请输入密码:\n");
    getchar();
    while ((ch = getch2()) != '\r')
    {
        if (ch == '\n')
            break;
        if (ch == BACKSPACE)
        {
            if (i > 0)
            {
                registername.passwd[i--] = '\0';
                putchar('\b');
                putchar(' ');
                putchar('\b');
                continue;
            }
            else
            {
                printf("\a");
                continue;
            }
        }
        else
        {

            tcsetattr(0, TCSANOW, &old);
            registername.passwd[i++] = ch;
            printf("*"); //在回显状态下输出*
        }
    }
    registername.passwd[i] = '\0';
    printf("\n请输入密保问题:\n");
    scanf("%s", registername.qusetion);
    printf("请输入密保答案:\n");
    scanf("%s", registername.answer);
    registername.flag = 1;
    send(conn_fd, &registername, sizeof(qq), 0);
    recv(conn_fd, &registername, sizeof(qq), 0);
    getchar();
    printf("%s\n", registername.record);
    // getchar();
    if (strcmp(registername.record, "账号申请成功") == 0)
    {
        client_login(conn_fd); //注册完成之后转入登陆界面
    }
}

//登陆函数
void client_login(int conn_fd)
{
    printf("\033c");
    qq login_name;
    printf("\t\t登陆页面\t\t\n");
    printf("-----------------------------------\n");
    printf("QQ:\n");
    scanf("%s", login_name.number);
    printf("passwd:\n");

    getchar();
    system("stty -echo");
    scanf("%s", login_name.passwd); //密码的隐式登陆
    system("stty echo");
    printf("login_name %s %s\n", login_name.number, login_name.passwd);
    login_name.flag = 2;
    send(conn_fd, &login_name, sizeof(qq), 0); //接受返回信息
    recv(conn_fd, &login_name, sizeof(qq), 0);
    getchar();
    printf("%s\n", login_name.record);
    memset(login_name.record, 0, sizeof(login_name.record));
    if (login_name.state == -1)
    {
        printf("密码错误，请重新登陆\n");
        return;
    }
    if (login_name.state == -2)
    {
        printf("用户已经登陆\n");
        return;
    }
    important_menu(login_name, conn_fd);
}

int logout(qq information, int conn_fd) //只发送下线信号
{
    information.state = 0;
    information.flag = 21;
    printf("您已经下线\n");
    send(conn_fd, &information, sizeof(qq), 0);
}

int my_recv(void)
{
    while (1)
    {
        qq information2;
        memset(&information2, 0, sizeof(qq));
        int t;
        t = recvn(my_connfd, &information2, sizeof(qq), 0);
        // printf("information2 size %d\n",information2.size);
        // if (t == 0)
        // {
        //     printf("服务器正在维护中!请稍候进行登陆\n");
        //     exit(0);
        // }
        if (t < 0)
        {
            my_err("recv", __LINE__);
            exit(0);
        }
        switch (information2.flag)
        {

        case 3:
        {
            printf("flag %d\n", information2.flag);
            //接受消息一个线程，发消息一个线程
            get_friend_news(information2);
            break;
        }
        case 4: //添加好友
        {
            if (information2.state != -8 && information2.state != -7)
            {
                printf("record %s \n", information2.record);
                printf("number %s to %s\n", information2.number, information2.to);
                printf("%s给您发来了添加好友的申请,请到消息盒子中处理\n", information2.number);
                add_news_box(information2); //添加到消息盒子之中
            }
            printf("information2.state %d\n", information2.state);
            if (information2.state == -8)
            {
                printf("对方已经同意你的好友申请\n");
            }
            else if (information2.state == -7)
            {
                printf("对方已经拒绝你的好友申请\n");
            }
            break;
        }
        case 5:
        {
            if (strcmp(information2.record, "表中暂无数据") == 0)
            {
                printf("%s\n", information2.record);
                break;
            }
            printf("my_friend number \t | my_firend_name\t | my_friend_state\n");
            if (information2.state == 0)
            {
                printf("\033[90m %s                     \t\033[0m", information2.to);
                printf("\033[90m %s                      \t\033[0m", information2.username);
                printf("off_line\t");
                printf("\n");
            }
            else
            {
                printf("\033[31m %s                       \t\033[0m ", information2.to);
                printf("\033[31m %s                       \t\033[0m ", information2.username);
                printf("on_line\t");
                printf("\n");
            }
            break;
        }

        case 6:
        {
            if (information2.state != -3)
            {
                printf("删除成功,友谊诚可贵!\n");
            }
            else
            {
                printf("删除失败,此人并不是你的好友!\n");
            }
            break;
        }
        case 10:
        {
            printf("\t\tmy_group name \t\t \n");
            printf("\t\t%s\t\t \n", information2.record);
            break;
        }
        case 11:
        {
            printf("my_group number \t | my_group_name\t | my_group_state\n");
            printf("\033[90m %s                     \t\033[0m", information2.number);
            printf("\033[90m %s                      \t\033[0m", information2.username);

            if (information2.state < 2)
            {
                printf("\033[90m 群员                      \t\033[0m");
                printf("\n");
            }
            if (information2.state == 3)
            {
                printf("\033[90m 群主                     \t\033[0m");
                printf("\n");
            }
            if (information2.state == 4)
            {
                printf("\033[90m 管理员                    \t\033[0m");
                printf("\n");
            }
            break;
        }
        case 12:
        {
            if (information2.state == -4)
            {
                printf("设置失败！\n");
            }
            if (information2.state == 3)
            {
                printf("设置成功，此人已经成为管理员\n");
            }
            break;
        }
        case 13:
        {
            if (information2.state == 1)
            {
                printf("%s\n", information2.record);
                add_news_box(information2); //添加到消息盒子之中
            }
            if (information2.state == -9)
            {
                printf("对方已经同意你的群申请\n");
            }
            else if (information2.state == -10)
            {
                printf("对方已经拒绝你的群申请\n");
            }
            break;
        }
        break;
        case 14:
        {
            if (information2.state == -2)
            {
                printf("此人不是群成员,无法删除!\n");
                break;
            }
            if (information2.state == 3)
            {
                printf("群主不能踢自己哦\n");
                break;
            }
            if (information2.state == 1)
            {
                printf("删除成功\n");
                break;
            }
            if (information2.state <= 2 && information2.state >= 0)
            {
                printf("你不是管理员,你没有权限！\n");
                break;
            }
            printf("删除成功!\n");

            break;
        }
        case 15:
        {
            get_chatting_group(information2);
            break;
        }

        case 16:
        {
            if (information2.state == -5)
            {
                printf("此成员不是我的好友,无法传送文件\n");
                good = -1;
            }
            break;
        }
        case 17:
        {
            printf("state %d \n", information2.state);
            if (information2.state == -6)
            {
                recv_file(information2, my_connfd);
                printf("写文件 >>%s<<\n", information2.record);
                //get_file_one(information2,my_connfd);
            }
            break;
        }
        case 18:
        {
            printf("好友%s给您发送了%s\n", information2.to, information2.record);
            break;
        }
        case 19:
        {
            if (strcmp(information2.record, "表中暂无数据") == 0)
            {
                printf("%s\n", information2.record);
                break;
            }
            printf("好友%s给好友%s发送了%s\n", information2.number, information2.to, information2.record);
            break;
        }
        case 20:
        {
            printf("群%s有消息:%s  \n", information2.to, information2.record);
            break;
        }
        default:
            break;
        }
    }
}

int my_news_box(qq information2, int conn_fd)
{
    char ch;
    char x;
    char z;
    qq *temporarily;
    qq *q1 = head;
    qq *temp = head->next;
    if (temp == NULL)
    {
        printf("  暂无新消息产生.请稍后再来 \n");
    }
    while (temp)
    {
        printf("flag %d\n", temp->flag);
        printf("temp %s\n", temp->record);
        switch (temp->flag)
        {
        case 3:
            printf("好友%s给您发来一条消息:%s\n", temp->number, temp->record);
            temporarily = temp;
            temp = temp->next;
            free(head);
            head = temporarily;
            break;
        case 4:
            printf("%s(y/n)\n", temp->record);
            //    getchar();
            scanf("%c", &x);
            getchar();
            if (x == 'y')
            {
                information2.state = -8;
            }
            else
            {
                information2.state = -7;
            }
            strcpy(information2.number, temp->number);
            strcpy(information2.to, temp->to);
            information2.flag = temp->flag;
            send(conn_fd, &information2, sizeof(qq), 0);
            temporarily = temp;
            temp = temp->next;
            free(head);
            head = temporarily;
            break;
        case 13:
            printf("%s(y/n)\n", temp->record);
            scanf("%c", &z);
            //    getchar();
            printf("y1 %c\n", z);
            if (z == 'y')
            {
                information2.state = -9;
            }
            if (z == 'n')
            {
                information2.state = -10;
            }
            printf("state %d \n", information2.state);
            getchar();
            strcpy(information2.username, temp->username);
            strcpy(information2.number, temp->number);
            strcpy(information2.to, temp->to);
            information2.flag = temp->flag;
            send(conn_fd, &information2, sizeof(qq), 0);
            temporarily = temp;
            temp = temp->next;
            free(head);
            head = temporarily;
            break;
        case 15:
            printf("群%s给您发来一条消息:%s\n", temp->username, temp->record);
            temporarily = temp;
            temp = temp->next;
            free(head);
            head = temporarily;
            break;
        case 17:
            printf("用户%s给您发了一个文件\n", temp->to);
            temporarily = temp;
            temp = temp->next;
            free(head);
            head = temporarily;
            break;
        default:
            break;
        }
    }
}

void important_menu(qq information, int conn_fd) //主菜单
{

    int choice = 0, menu_flag = 1, i = 0;
    //进行实际连接
    my_connfd = conn_fd;
    pthread_t thid;
    pthread_create(&thid, NULL, (void *)my_recv, NULL);
    system("clear");
    do
    {
        signal(SIGPIPE, SIG_IGN);
        main_menu(menu_flag);
        int *get = (int *)malloc(200);
        while (1)
        {
            get[i] = getch1();
            if (get[i] == '\n')
                break;
            if (get[i] == 0x41)
            {
                choice--;
                menu_flag--;
                system("clear");
                main_menu(menu_flag);
            }
            if (get[i] == 0x42)
            {
                choice++;
                menu_flag++;
                system("clear");
                main_menu(menu_flag);
            }
            i++;
        }
        free(get);
        printf("choice %d\n", choice);
        switch (choice)
        {
        case 1:
        {
            my_friend(information, conn_fd);
            break;
        }
        case 2:
        {
            my_group(information, conn_fd);
            break;
        }
        case 3:
        {
            my_news_box(information, conn_fd);
            break;
        }
        case 4:
        {
            my_onlie_news(information, conn_fd);
            break;
        }
        case 5:
        {
            logout(information, conn_fd);
            return;
        }
        }
    } while (choice >= 1 && choice <= 4);
}

//查看我的离线消息
void my_onlie_news(qq information, int conn_fd)
{
    information.flag = 18;
    send(conn_fd, &information, sizeof(qq), 0);
}

void my_group(qq information, int conn_fd)
{
    int choice = 0, group_flag = 1, i = 0;
    //进行实际连接
    system("clear");
    do
    {
        signal(SIGPIPE, SIG_IGN);
        group_menu(group_flag);
        int *get = (int *)malloc(200);
        while (1)
        {
            get[i] = getch1();
            if (get[i] == '\n')
                break;
            if (get[i] == 0x41)
            {
                choice--;
                group_flag--;
                system("clear");
                group_menu(group_flag);
            }
            if (get[i] == 0x42)
            {
                choice++;
                group_flag++;
                system("clear");
                group_menu(group_flag);
            }
            i++;
        }
        free(get);
        switch (choice)
        {
        case 1:
        {
            my_create_group(information, conn_fd);
            break;
        }
        case 2:
        {
            my_enter_group(information, conn_fd);
            break;
        }
        case 3:
        {
            my_delete_group(information, conn_fd);
            break;
        }
        case 4:
        {
            my_group_list(information, conn_fd);
            break;
        }
        case 5:
        {
            break;
        }
        }
    } while (choice >= 1 && choice <= 4);
}
//分菜单关于进入群聊的判断
void enter_menu()
{
    printf("选择1 进入群聊.\n");
    printf("选择2 邀请他人进入此群聊\n");
    printf("选择3 让某个成员离开此群聊\n");
    printf("选择4 设置管理员(只有群主才有权限)\n");
    printf("选择5 查看群成员\n");
    printf("选择6 查看群聊天记录\n");
    printf("选择0 返回上层\n");
}

void member_list(qq information, int conn_fd) //查看群成员
{
    information.flag = 11;
    int t = 9999;
    send(conn_fd, &information, sizeof(qq), 0);
}

void set_guanli(qq information, int conn_fd) //设置管理员
{

    printf("请输入你想要设置为管理员的成员账号\n");
    scanf("%s", information.to); //

    information.flag = 12;

    send(conn_fd, &information, sizeof(information), 0);
}

void inivt_merber(qq information, int conn_fd) //邀请成员进群
{
    information.flag = 13; //设置flag方便服务器响应

    char name[5];

    printf("请输入你想要邀请的人进入此群聊\n");
    scanf("%s", information.to);
    send(conn_fd, &information, sizeof(qq), 0);
}

void kick_out_members(qq information, int conn_fd) //踢人
{
    printf("请输入你想要踢的人\n");
    scanf("%s", information.to);
    char c;
    printf("您确定要踢这个成员吗，友谊可贵\n");
    getchar();
    scanf("%c", &c);
    if (c != 'y')
    {
        return;
    }
    information.flag = 14; //选择标志位
    send(conn_fd, &information, sizeof(qq), 0);
}

void chatting_group(qq information, int conn_fd) //一对多的聊天
{
    information.flag = 15; //标志位
    printf("用户:%s(quit退出):   \n", information.number);
    scanf("%s", information.record);
    while (strcmp(information.record, "quit") != 0)
    {
        send(conn_fd, &information, sizeof(qq), 0);
        printf("用户:%s(quit退出):   \n", information.number);
        scanf("%s", information.record);
    }
}

void get_chatting_group(qq information)
{
    if (strcmp(group_num, information.username) == 0)
    {
        printf("群%s : %s\n", information.username, information.record);
    }
    else
    {
        printf("群%s给你发来一条信息,请到消息盒子进行处理\n", information.username);
        add_news_box(information);
    }
}

void my_enter_group(qq information, int conn_fd)
{
    printf("\033c");
    int num;
    information.flag = 9;
    printf("请选择你想要进入的群聊\n");
    scanf("%s", information.username);
    scanf("%s", information.qusetion);
    strcpy(group_num, information.username);
    printf(" %s \n", group_num);
    do
    {
        enter_menu();
        scanf("%d", &num);
        switch (num)
        {
        case 0:
            my_group(information, conn_fd);
            break;
        case 1:
            chatting_group(information, conn_fd);
            break;
        case 2:
            inivt_merber(information, conn_fd);
            break;
        case 3:
            kick_out_members(information, conn_fd);
            break;
        case 4:
            set_guanli(information, conn_fd);
            break;
        case 5:
            member_list(information, conn_fd);
            break;
        case 6:
            group_news_list(information, conn_fd);
            break;
        }

    } while (num >= 1 && num <= 6);
}
void my_delete_group(qq information, int conn_fd)
{
    char c;
    printf("请输入你想要解散的群(名称)\n");
    scanf("%s", information.username);
    printf("你确定要解散本群吗!(y/n)\n");

    getchar();
    scanf("%c", &c);
    getchar();
    if (c != 'y')
    {
        return;
    }
    information.flag = 9;
    send(conn_fd, &information, sizeof(qq), 0);
}

void my_group_list(qq information, int conn_fd)
{
    information.flag = 10;
    int t = 9999;
    send(conn_fd, &information, sizeof(qq), 0);
}

void my_create_group(qq information, int conn_fd)
{
    char username[13];
    information.flag = 8;
    printf("请输入群名称\n");
    scanf("%s", information.username);
    strcpy(username, information.username);
    information.state = 3;                      //群主的套接字为3
    send(conn_fd, &information, sizeof(qq), 0); //整个结构发过去
    do
    {
        printf("请输入想要添加的成员账号(quit退出)\n");
        scanf("%s", information.to);
        information.size = 100;
        information.state = 1;
        information.flag = 8;
        strcpy(information.username, username);
        printf("username %s \n", information.username);
        send(conn_fd, &information, sizeof(qq), 0);
    } while (strcmp(information.to, "quit") != 0);
    getchar();
}

void init_file(qq information, int conn_fd)
{
    system("clear");
    printf("\t请输入你想要发送文件的好友\t\n");
    scanf("%s", information.to);
    information.flag = 16;

    send(conn_fd, &information, sizeof(qq), 0);
    if (good != -1)
        send_file(information, conn_fd);
}

void my_friend(qq information, int conn_fd)
{

    int choice = 0, friend_flag = 1, i = 0;
    //进行实际连接
    // system("clear");
    do
    {
        signal(SIGPIPE, SIG_IGN);
        friend_menu(friend_flag);
        int *get = (int *)malloc(200);
        while (1)
        {
            get[i] = getch1();
            if (get[i] == '\n')
                break;
            if (get[i] == 0x41)
            {
                choice--;
                friend_flag--;
                // system("clear");
                friend_menu(friend_flag);
            }
            if (get[i] == 0x42)
            {
                choice++;
                friend_flag++;
                // system("clear");
                friend_menu(friend_flag);
            }
            i++;
        }
        free(get);
        switch (choice)
        {
        case 0:
        {

            break;
        }
        case 1:
        {
            add_myfriend(information, conn_fd);
            break;
        }
        case 2:
        {
            delete_myfriend(information, conn_fd);
            break;
        }
        case 3:
        {
            view_myfriend(information, conn_fd);
            break;
        }
        case 4:
        {
            view_black(information, conn_fd);
            break;
        }
        case 5:
        {
            init_file(information, conn_fd);
            break;
        }
        case 6:
        {
            view_list(information, conn_fd);
            break;
        }
        case 7:
        {
            view_news(information, conn_fd);
            break;
        }
        case 8:
        {
            break;
        }
        }
    } while (choice >= 1 && choice <= 7);
}

void view_black(qq information, int conn_fd)
{
    information.flag = 7;
    printf("请输入你要屏蔽的账号\n");
    scanf("%s", information.to);
    getchar();
    send(conn_fd, &information, sizeof(qq), 0);
}

void delete_myfriend(qq information, int conn_fd)
{

    char name[3];
    char x;
    printf("Please input the friend you want to delete.\n");
    scanf("%s", information.to);
    information.flag = 6;

    printf("Are you sure you want to delete this friend?(y/n)\n");
    getchar();
    scanf("%c", &x);
    if (x != 'y')
    {
        return;
    }
    getchar();
    send(conn_fd, &information, sizeof(qq), 0);
}

void add_myfriend(qq information, int conn_fd)
{

    printf("请输入你想要添加的好友:\n");
    scanf("%s", information.to);
    information.flag = 4;
    getchar(); //把回车接受不然会影响方向键
    send(conn_fd, &information, sizeof(qq), 0);
}

//注意send和recv匹配问题
//2个send应该对应两个recv

//查看好友列表
void view_list(qq information, int conn_fd)
{
    information.flag = 5;
    int t = 9999;
    send(conn_fd, &information, sizeof(qq), 0);
}

void view_myfriend(qq information, int conn_fd)
{

    pthread_t tid2;
    information.flag = 3;
    printf("information flag %d\n", information.flag);
    printf("请输入你想要聊天的账户\n");
    scanf("%s", information.to);
    getchar();
    strcpy(to, information.to); //对于聊天情况进行判断
    strcpy(information.record, " ");
    send(conn_fd, &information, sizeof(qq), 0);
    //读写冲突,主线程已经结束
    //如果同时创建两个线程,
    my_connfd = conn_fd;
    printf("\033c");
    while (1)
    {
        printf("pleases chatting_with your_friend(输入quit，退出聊天)\n");
        printf("to %s\n", to);
        scanf("%s[^\n]", information.record);
        if (strcmp(information.record, "quit") == 0)
        {
            getchar();
            break;
        }
        if (send(my_connfd, &information, sizeof(qq), 0) < 0)
        {
            my_err("send", __LINE__);
        }
    }
}

//发消息一个线程
//两个人的聊天记录利用结构体存储起来，不然每次发给服务器，会重新接受一次
//会产生错误

void get_friend_news(qq information)
{
    if (information.state == -1)
    {
        printf("对方不是你的好友，无法聊天\n");
        return;
    }
    if (information.state == -2)
    {
        printf("对方已经把你拉黑\n");
        return;
    }
    if (strcmp(to, information.to) == 0)
    {
        printf("好友%s:  %s\n", information.to, information.record);
    }
    else
    {
        printf("好友%s: 给您发来一条新消息，请您到消息盒子来进行处理\n", to);
        add_news_box(information);
    }
}

int main()
{
    signal(SIGPIPE, SIG_IGN);

    int conn_fd;
    struct sockaddr_in serv_addr;
    init();
    memset(&serv_addr, 0, sizeof(struct sockaddr_in));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);
    inet_aton("128.0.0.185", &serv_addr.sin_addr);
    //创建一个TCP套接字
    conn_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (conn_fd < 0)
    {
        my_err("conn_fd", __LINE__);
    }
    if (connect(conn_fd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) < 0)
    {
        my_err("conn_fd", __LINE__);
    }
    int choice = 0, flag = 3, i = 0;
    //进行实际连接
    system("clear");
    do
    {
        menu(flag);
        int *get = (int *)malloc(200);
        while (1)
        {
            get[i] = getch1();
            if (get[i] == '\n')
                break;
            if (get[i] == 0x41)
            {
                choice--;
                flag--;
                system("clear");
                menu(flag);
            }
            if (get[i] == 0x42)
            {
                choice++;
                flag++;
                system("clear");
                menu(flag);
            }
            i++;
        }
        free(get);
        switch (choice)
        {
        case 1:
        {
            regist(conn_fd);
            break;
        }
        case 2:
        {
            client_login(conn_fd);
            break;
        }
        case 3:
            return 0;
        }
    } while (choice >= 0 && choice <= 3);
    return 0;
}
