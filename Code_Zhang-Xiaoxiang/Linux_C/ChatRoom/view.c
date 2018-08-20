#include "base.h"
#include "view.h"

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

int vRegister(void)
{
    int count = 0, ch, flag = 0;
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
    // 调用mysql添加新用户
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
        printf("\t\t[7] 发送文件\t\t\t\n");
        printf("\t\t[8] 退出登录\t\t\t\n");
        printf("\t\t[9] 退出软件\t\t\t\n");
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
