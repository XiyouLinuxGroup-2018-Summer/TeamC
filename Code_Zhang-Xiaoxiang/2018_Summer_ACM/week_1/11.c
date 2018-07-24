/**
* 11.c
* @author: PhoenixXC
* @email: xuancbm@xiyoulinux.org
* @description: K - 素数 
* @created: Tue Jul 24 2018 09:16:08 GMT+0800 (CST)
* @last-modified: Tue Jul 24 2018 09:16:08 GMT+0800 (CST)
*/

#include <stdio.h>
int main(void)
{
    int x, y;
    while (scanf("%d %d", &x, &y) == 2 && !(x == 0 && y == 0))
    {
        int flag = 0;
        for (int i = x; i <= y; i++)
        {
            int k = i * i + i + 41;
            for (int j = 2; j * j <= k; j++)
            {
                if (k % j == 0 || j * j == k)
                {
                    flag = 1;
                    break;
                }
            }
            if (flag == 1)
                break;
        }
        if (flag != 1)
            printf("OK\n");
        else
            printf("Sorry\n");
    }
}