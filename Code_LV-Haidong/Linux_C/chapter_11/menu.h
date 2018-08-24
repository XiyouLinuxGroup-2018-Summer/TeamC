#pragma once
#ifndef MENU_H_
#define MENU_H_
#include <stdio.h>
#include <stdlib.h>
void menu(int flag)
{
    //printf("\033c");
    int i = 0;
    char map[6][100] =
        {
            "    \033[34m     欢迎来到My-Room聊天室                  \n",
            "                                                      \n",
            "                                                      \n",
            "    \033[31m     1.注册                                \n",
            "    \033[34m     2.登陆                                \n",
            "    \033[35m     3.退出程序                 \033[37m    \n",
        };
    for (i = 0; i < 6; i++)
    {
        if (flag == i + 1)
            printf("-->"); //如果相等箭头输出
        else
            printf("  "); //不然输出空格
        printf("%s", map[i]);
    }
}
#endif
