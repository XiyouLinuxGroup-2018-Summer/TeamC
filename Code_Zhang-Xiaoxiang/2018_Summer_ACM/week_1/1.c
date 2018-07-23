/**
* 1.c
* @author: PhoenixXC
* @email: xuancbm@xiyoulinux.org
* @description: A - 字符排序
* @created: Mon Jul 23 2018 15:45:27 GMT+0800 (CST)
* @last-modified: Mon Jul 23 2018 15:45:27 GMT+0800 (CST)
*/

#include <stdio.h>
#include <stdlib.h>
int main(void)
{
    char ch[4];
    while (scanf("%s", ch) == 1)
    {
        int i, j, k;
        int min = 0;
        for (i = 0; i < 3; i++)
        {
            for (j = 0; j < 3; j++)
            {
                if (ch[j] == ' ')
                    continue;
                if (ch[min] > ch[j])
                    min = j;
            }
            printf("%c", ch[min]);
            ch[min] = ' ';
            for (k = 0; k < 3;k++)
            {
                if (ch[k] != ' ')
                {
                    min = k;
                    break;
                }
            }
            if (i != 2)
                printf(" ");
            else
                printf("\n");
        }
    }
}