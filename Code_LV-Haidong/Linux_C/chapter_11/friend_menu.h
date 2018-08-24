#pragma once
#ifndef _FRIEND_MENU_H_
#define _FRIEND_MENU_H_
#include <stdio.h>
#include <stdlib.h>
void friend_menu(int friend_flag)
{
    printf("\033c");
    int i = 0;
    char map[10][100] =
        {
            "                                                                \n",
            "    \033[34m     1.添加好友                                       \n",
            "    \033[31m     2.删除好友                                       \n",
            "    \033[34m     3.与好友发私信                                    \n",
            "    \033[31m     4.查看黑名单                                      \n",
            "    \033[31m     5.发送文件                                       \n",
            "    \033[34m     6.查看好友列表                                    \n",
            "    \033[31m     7.查看好友消息记录                                 \n",
            "    \033[35m     8.退出程序                       \033[37m         \n",
        };
    for (i = 0; i < 10; i++)
    {
        if (friend_flag == i + 1)
            printf("-->"); //如果相等箭头输出
        else
            printf("  "); //不然输出空格
            printf("%s", map[i]);
    }
}
#endif