/**
* 3.c
* @author: PhoenixXC
* @email: xuancbm@xiyoulinux.org
* @description: C - 回文串 
* @created: Mon Jul 23 2018 15:59:11 GMT+0800 (CST)
* @last-modified: Mon Jul 23 2018 15:59:11 GMT+0800 (CST)
*/

#include <stdio.h>
#include <string.h>

int main(void)
{
    int n, i;
    char str[105];

    scanf("%d", &n);
    for (i = 0; i < n; i++)
    {
        scanf("%s", str);
        int len = strlen(str), flag = 0;
        for (int k = 0; k < len / 2; k++)
        {
            if (str[k] != str[len - k - 1])
            {
                printf("no\n");
                flag = 1;
                break;
            }
        }
        if (!flag)
            printf("yes\n");
    }
}