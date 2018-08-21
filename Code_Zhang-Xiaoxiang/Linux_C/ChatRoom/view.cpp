#include "base.h"
#include "view.h"
// #include "account.h"
#include "internet.h"

int vBeginRLQ(void)
{
    int ch;
    S_CLEAR(); S_BLOD(); S_COLOR(40, 33);
    printf("*------------------- Chat Room -------------------*\n\n");
    S_CLOSE();
    do
    {
        printf("\t\t     [1] 注册     \t\t\n");
        printf("\t\t     [2] 登录     \t\t\n");
        printf("\t\t     [3] 退出     \t\t\n");
        printf("\t\t     [4] 关于     \t\t\n");
        S_COLOR(40, 33);
        printf("\n*-------------------------------------------------*\n\n");
        S_CLOSE();
        scanf("%d", &ch);
        if (ch >= 1 && ch <= 4)
            return ch;
        while(getchar() != '\n');
        S_CLEAR(); S_BLOD(); S_COLOR(40, 33);
        printf("*------------------- Chat Room -------------------*\n\n");
        S_CLOSE();
    }while(1);
}

int vRegister(Package * data, int sockfd)
{
    int ret;
    Package recvpack;
    memset(&recvpack, 0, sizeof(recvpack));
    char username[USER_NAME_MAX + 1], pass[USER_PASS_MAX + 1], passagain[USER_PASS_MAX + 1];
    char question[OTHER_SIZE + 1], answer[OTHER_SIZE + 1];
    while(1)
    {
        do
        {
            S_CLEAR(); S_BLOD(); S_COLOR(40, 33);
            printf("*------------------- Chat Room -------------------*\n");
            printf("*                   - register -                  *\n\n");
            S_CLOSE();
            printf("           please input username[1 ~ %d]:         \n", USER_NAME_MAX);
            printf("\t\tusername: ");
        }while(!s_gets(username, USER_NAME_MAX, stdin) || username[0] == '\n');

        printf("           please input password[%d ~ %d]:        \n", USER_PASS_MIN, USER_PASS_MAX);
        printf("\t\tpassword: ");
        S_SAVEPOS();

        while (!s_getchs(pass, USER_PASS_MAX) || strlen(pass) < USER_PASS_MIN)
            S_RESTPOS();
        printf("\n");  
        // repeat password
        printf("           please input password again[%d ~ %d]:        \n", USER_PASS_MIN, USER_PASS_MAX);
        printf("\t\tpassword: ");
        S_SAVEPOS();
        while (!s_getchs(passagain, USER_PASS_MAX) || strlen(passagain) < USER_PASS_MIN)
            S_RESTPOS();
        printf("\n");
        
        if (strcmp(pass, passagain) != 0)
        {
            printf("[ERROR] -- 两次输入的密码不一致\n");   
            sleep(1);
        }
        else
        {
            printf(" please input question for retrieving your password\n");
            printf("\t\tQUESTION: ");
            s_gets(question, OTHER_SIZE, stdin);
            printf("           please input the answers of question\n");
            printf("\t\tANSWER: ");
            s_gets(answer, OTHER_SIZE, stdin);
            break;
        }
    }
    // 封装pack
    memset(data, 0, PACK_SIZE);
    sprintf(data->strmsg, "%s%s%s%s%s%s%s%s", username, _END_, pass, _END_, question, _END_, answer, _END_);
    ret = SendMSG(sockfd, data, PACK_SIZE, 0);
    if (ret < 0)
        my_err(__FILE__, "SendMSG", __LINE__);
    ret = RecvMSG(sockfd, &recvpack, PACK_SIZE, 0);
    if (ret < 0)
        my_err(__FILE__, "RecvMSG", __LINE__);
    return recvpack.cmdflag;
}

void vAbout(void)
{
    S_CLEAR(); S_BLOD(); S_COLOR(40, 33);
    printf("*------------------- Chat Room -------------------*\n");
    printf("*                    - ABout -                    *\n");
    S_CLOSE();      S_COLOR(40, 33);
    printf("*                                                 *\n");
    printf("*                一个简陋的聊天室 ;)              *\n");
    printf("*                                                 *\n");    
    printf("*-------------------------------------------------*\n");
    S_CLOSE();
    printf("\n按任意键返回...\n");
    getchar();
}

void vMessageTop(int num)
{
    char msg[30];
    if (num == 0)
        sprintf(msg, "~~ 您没有新的消息");
    else
        sprintf(msg, "您收到了%2d 条消息", num);
    S_BLOD();
    printf("*------------------- ▶ Message ◀ -------------------*\n\n");
    S_CLOSE();
    printf("*\t\t  ");
    if (num)    S_FLCK();
    printf("%s", msg);
    S_CLOSE();
    printf("\t\t    *\n\n");
    S_BLOD();
    printf("*---------------------------------------------------*\n\n");
    S_CLOSE();
}

int vFunMainMenu(void)
{
    int cmd;
    do
    {
        S_CLEAR(); S_BLOD(); S_COLOR(40, 33);
        printf("                  - Function -                   \n\n");    
        S_CLOSE();
        printf("\t\t[1] 查看离线消息\t\t\t\n");
        // [system] && [friend and group]
        printf("\t\t[2] 查看好友列表\t\t\t\n");
        printf("\t\t[3] 查看群组列表\t\t\t\n");
        printf("\t\t[4] 添加好友\t\t\t\n");
        printf("\t\t[5] 添加群聊\t\t\t\n");
        printf("\t\t[6] 创建群聊\t\t\t\n");
        printf("\t\t[7] 退出登录\t\t\t\n");
        printf("\t\t[8] 退出软件\t\t\t\n");
        S_BLOD(); S_COLOR(40, 33);
        printf("\n-------------------------------------------------\n");
        S_CLOSE();
        printf("\nYour Choice is :\t");
        scanf("%d", &cmd);
        while (getchar() != '\n');
        if (cmd > 0 && cmd <= 8)
            return cmd;
    }while(1);
}

int vLogin(Package * data, int sockfd)
{
    int userid, ret;
    char pass[USER_PASS_MAX + 1];
    Package recvpack;
    while(1)
    {
        S_CLEAR(); S_BLOD(); S_COLOR(40, 33);
        printf("*------------------- Chat Room -------------------*\n");
        printf("*                    - login -                    *\n\n");
        S_CLOSE();
        printf("                please input userid:         \n");
        printf("\t\tusername: ");
        if (scanf("%d", &userid) != 1)
            while(getchar() != '\n');
        else
            break;
    }

    printf("           please input password[%d ~ %d]:        \n", USER_PASS_MIN, USER_PASS_MAX);
    printf("\t\tpassword: ");
    S_SAVEPOS();

    while (!s_getchs(pass, USER_PASS_MAX) || strlen(pass) < USER_PASS_MIN)
        S_RESTPOS();
    printf("\n");  
    
    memset(data, 0, PACK_SIZE);
    data->cmdflag = Flag_Cmd_Login;
    sprintf(data->strmsg, "%s%s%s%s", userid, _END_, pass, _END_);
    
    ret = SendMSG(sockfd, data, PACK_SIZE, 0);
    if (ret < 0)
        my_err(__FILE__, "SendMSG", __LINE__);
    ret = RecvMSG(sockfd, &recvpack, PACK_SIZE, 0);
    if (ret < 0)
        my_err(__FILE__, "RecvMSG", __LINE__);
    return recvpack.cmdflag;
}

// int vForgetPass(char * question, char * answer)
// {
//     for (int i = 0; i < 3; i++)
//     {
//         S_CLEAR(); S_BLOD(); S_COLOR(40, 33);
//         printf("*------------------- Chat Room -------------------*\n");
//         printf("*              - Retrieve password -              *\n\n");
//         S_CLOSE();
//         printf("          [%16s]\n", question);
//         printf("ANSWER: ");
//         memset(answer, 0, sizeof(answer));
//         s_gets(answer, OTHER_SIZE + 1, stdin);
//         // send to server 
//         // if (/* ... */)
//         // {
//         //     /* ... */
//         // }
//         // else
//         // {
//         //     /* ... */
//         // }
//     }
// }

int vFunFriendMenu(void)
{
    int cmd;
    do
    {
        S_CLEAR(); S_BLOD(); S_COLOR(40, 33);
        printf("                  - Function -                   \n\n");    
        S_CLOSE();
        printf("\t\t[1] 和ta聊天\t\t\t\n");
        printf("\t\t[2] 查看聊天记录\t\t\t\n");
        printf("\t\t[3] 加入黑名单\t\t\t\n");
        printf("\t\t[4] 移出黑名单\t\t\t\n");
        printf("\t\t[5] 删除联系人\t\t\t\n");
        printf("\t\t[6] 邀请好友加群\t\t\t\n");
        printf("\t\t[7] 发送文件\t\t\t\n");
        printf("\t\t[8] 返回\t\t\t\n");
        S_BLOD(); S_COLOR(40, 33);
        printf("\n-------------------------------------------------\n");
        S_CLOSE();
        printf("\nYour Choice is :\t");
        scanf("%d", &cmd);
        while (getchar() != '\n');
        if (cmd > 0 && cmd <= 8)
            return cmd;
    }while(1); 
}
