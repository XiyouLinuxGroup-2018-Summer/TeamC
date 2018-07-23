/**
* 7.c
* @author: PhoenixXC
* @email: xuancbm@xiyoulinux.org
* @description: G - 数字序列插入
* @created: Mon Jul 23 2018 16:50:21 GMT+0800 (CST)
* @last-modified: Mon Jul 23 2018 16:50:21 GMT+0800 (CST)
*/

#include <stdio.h>
#define maxn 100
int a[maxn];
int main(void)
{
    int n, m;
    while (scanf("%d %d", &n, &m) == 2 && !(m == 0 && n == 0))
    {
        int flag = 0;
        for (int i = 0; i < n; i++)
            scanf("%d", &a[i]);
        for (int i = 0; i < n; i++)
        {
            if (!flag && m < a[i])
            {
                printf("%d ", m);
                flag = 1;
            }
            printf("%d", a[i]);
            if (flag && i == n - 1)
                printf("\n");
            else
                printf(" ");
        }
        if (!flag)
            printf("%d\n", m);
    }
}