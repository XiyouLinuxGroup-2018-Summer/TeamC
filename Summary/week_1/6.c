/**
* 6.c
* @author: PhoenixXC
* @email: xuancbm@xiyoulinux.org
* @description: F - 数字字符统计 
* @created: Mon Jul 23 2018 16:44:14 GMT+0800 (CST)
* @last-modified: Mon Jul 23 2018 16:44:14 GMT+0800 (CST)
*/

#include <stdio.h>
#include <string.h>
int main(void)
{
    int n;
    scanf("%d", &n);
    getchar();
    char str[305];
    for(int i = 0; i < n; i++)
    {
        gets(str);
        int len = strlen(str), count = 0;
        for (int j = 0; j < len; j++)
        {
            if (str[j] >= '0' && str[j] <= '9')
                count++;
        }
        printf("%d\n", count);
    }
}