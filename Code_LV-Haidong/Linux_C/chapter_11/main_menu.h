#pragma once
#ifndef _MAIN_MENU_H_
#define _MAIN_MENU_H_
#include <stdio.h>
#include <stdlib.h>
void main_menu(int menu_flag)
{
    
    int i = 0;
    char map[6][100] =
        {
            "                                                               \n",
            "    \033[34m     1.我的好友                                     \n",
            "    \033[31m     2.我的群                                       \n",
            "    \033[34m     3.消息盒子                                     \n",
            "    \033[31m     4.查看离线消息                                  \n",
            "    \033[35m     5.退出程序                     \033[37m         \n",
        };
    for (i = 0; i < 6; i++)
    {
        if (menu_flag == i + 1)
            printf("-->"); //如果相等箭头输出
        else
            printf("  "); //不然输出空格
            printf("%s", map[i]);
    }
}
#endif