#pragma once
#ifndef _GROUP_MENU_H_
#define _GROUP_MENU_H_
#include <stdio.h>
#include <stdlib.h>
void group_menu(int group_flag)
{
    int i = 0;
    char map[8][100] =
        {
            "                                                                \n",
            "    \033[34m     1.创建群                                        \n",
            "    \033[34m     2.进入群聊                                      \n",
            "    \033[31m     3.解散群                                        \n",
            "    \033[34m     4.查看群列表                                     \n",
            "    \033[31m     5.返回上层              \033[37m                 \n",
        };
    for (i = 0; i < 6; i++)
    {
        if (group_flag == i + 1)
            printf("-->"); //如果相等箭头输出
        else
            printf("  "); //不然输出空格
            printf("%s", map[i]);
    }
}
#endif