/**
* 8.c
* @author: PhoenixXC
* @email: xuancbm@xiyoulinux.org
* @description: H - 合法标识符 
* @created: Mon Jul 23 2018 17:02:10 GMT+0800 (CST)
* @last-modified: Mon Jul 23 2018 17:02:10 GMT+0800 (CST)
*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>

int main(void)
{
    int t;
    char str[60];

    scanf("%d", &t);
    getchar();

    for (int i = 0; i < t; i++)
    {
        int flag = 0, j = 1;        
        gets(str);
        int len = strlen(str);
        if (isdigit(str[0]))
            flag = 1;
        else if (isalnum(str[0]) == 0 && str[0] != '_')
                    flag = 1;
        else
        {
            while (j < len)
            {
                if (isalnum(str[j]) == 0 && str[j] != '_')
                    flag = 1;
                j++;
            }
        }

        if (flag == 1)
            printf("no\n");
        else
            printf("yes\n");
    }
}