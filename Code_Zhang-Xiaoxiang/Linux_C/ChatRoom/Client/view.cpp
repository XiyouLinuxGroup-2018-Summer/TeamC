#include "../Base/base.h"
#include "view.h"
#include "../Base/internet.h"

int vBeginRLQ(void)
{
    int ch;
    CLEAR(); 
    //S_BLOD(); S_COLOR(40, 33);
    printf("*------------------- Chat Room -------------------*\n\n");
    // S_CLOSE();
    do
    {
        printf("\t\t     [1] 注册     \t\t\n");
        printf("\t\t     [2] 登录     \t\t\n");
        printf("\t\t     [3] 退出     \t\t\n");
        printf("\t\t     [4] 关于     \t\t\n");
        // S_COLOR(40, 33);
        printf("\n*-------------------------------------------------*\n\n");
        // S_CLOSE();
        scanf("%d", &ch);
        if (ch >= 1 && ch <= 4)
            return ch;
        while(getchar() != '\n');
        CLEAR(); 
        // S_BLOD(); S_COLOR(40, 33);
        printf("*------------------- Chat Room -------------------*\n\n");
        // S_CLOSE();
    }while(1);
}

int vRegister(Package * data, int sockfd)
{
    int ret;
    Package recvpack;
    memset(&recvpack, 0, sizeof(recvpack));
    char username[USER_NAME_MAX + 10], pass[USER_PASS_MAX + 10], passagain[USER_PASS_MAX + 10];
    char question[OTHER_SIZE + 1], answer[OTHER_SIZE + 1];
    while(1)
    {
        do
        {
            CLEAR(); 
            // S_BLOD(); S_COLOR(40, 33);
            printf("*------------------- Chat Room -------------------*\n");
            printf("*                   - register -                  *\n\n");
            // S_CLOSE();
            printf("           please input username[1 ~ %d]:         \n", USER_NAME_MAX);
            printf("\t\tusername: ");
        }while(!s_gets(username, USER_NAME_MAX, stdin) || username[0] == '\0');

        printf("           please input password[%d ~ %d]:        \n", USER_PASS_MIN, USER_PASS_MAX);
        printf("\t\tpassword: ");
        SAVEPOS();

        while (!s_getchs(pass, USER_PASS_MAX) || strlen(pass) < USER_PASS_MIN)
            RESTPOS();
        printf("\n");  
        // repeat password
        printf("           please input password again[%d ~ %d]:        \n", USER_PASS_MIN, USER_PASS_MAX);
        printf("\t\tpassword: ");
        SAVEPOS();
        while (!s_getchs(passagain, USER_PASS_MAX) || strlen(passagain) < USER_PASS_MIN)
            RESTPOS();
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
    sprintf(data->strmsg, "%s%s%s%s%s%s%s%s", TranToEsc(username), _END_, TranToEsc(pass), _END_, TranToEsc(question), _END_, TranToEsc(answer), _END_);
    data->cmdflag = Flag_Cmd_Register;
    ret = SendMSG(sockfd, data, PACK_SIZE, 0);
    if (ret < 0)
        my_err(__FILE__, "SendMSG", __LINE__);
    ret = RecvMSG(sockfd, &recvpack, PACK_SIZE, 0);
    if (ret < 0)
        my_err(__FILE__, "RecvMSG", __LINE__);
    return recvpack.statusflag;
}

int vLogin(Package * data, int sockfd)
{
    int userid, ret;
    char pass[USER_PASS_MAX + 1];
    Package recvpack;
    while(1)
    {
        CLEAR(); 
        // S_BLOD(); S_COLOR(40, 33);
        printf("*------------------- Chat Room -------------------*\n");
        printf("*                    - login -                    *\n\n");
        // S_CLOSE();
        printf("                please input userid:         \n");
        printf("\t\tuserid: ");
        if (scanf("%d", &userid) != 1)
            while(getchar() != '\n');
        else
            break;
    }
    while(getchar() != '\n');
    printf("           please input password[%d ~ %d]:        \n", USER_PASS_MIN, USER_PASS_MAX);
    printf("\t\tpassword: ");
    SAVEPOS();

    while (!s_getchs(pass, USER_PASS_MAX) || strlen(pass) < USER_PASS_MIN)
        RESTPOS();
    printf("\n");  
    
    data->source_id = userid;
    data->cmdflag = Flag_Cmd_Login;
    sprintf(data->strmsg, "%d%s%s%s", userid, _END_, pass, _END_);
    
    ret = SendMSG(sockfd, data, PACK_SIZE, 0);
    if (ret < 0)
        my_err(__FILE__, "SendMSG", __LINE__);
        
    ret = RecvMSG(sockfd, &recvpack, PACK_SIZE, 0);
    if (ret < 0)
        my_err(__FILE__, "RecvMSG", __LINE__);
    if (recvpack.statusflag == 0)
    {
        fprintf(stderr, "%s", recvpack.strmsg);
        return 0;
    }
    return userid;
}

void vAbout(void)
{
    CLEAR(); 
    //S_BLOD(); S_COLOR(40, 33);
    printf("*------------------- Chat Room -------------------*\n");
    printf("*                    - ABout -                    *\n");
    // S_CLOSE();      S_COLOR(40, 33);
    printf("*                                                 *\n");
    printf("*                一个简陋的聊天室 ;)              *\n");
    printf("*                                                 *\n");    
    printf("*-------------------------------------------------*\n");
    // S_CLOSE();
    printf("\n按任意键返回...\n");
    getchar();
}
