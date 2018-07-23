/**
* 5.c
* @author: PhoenixXC
* @email: xuancbm@xiyoulinux.org
* @description: E - 计算器 
* @created: Mon Jul 23 2018 16:19:56 GMT+0800 (CST)
* @last-modified: Mon Jul 23 2018 16:19:56 GMT+0800 (CST)
*/

#include <stdio.h>
int main(void)
{
    int T, i;
    scanf("%d", &T);
    getchar();
    for (i = 0; i < T; i++)
    {
        char ch;
        long a, b, c;
        double d;
        ch = getchar();
        scanf("%ld %ld", &a, &b);
        getchar();
        if (ch == '+')
            printf("%ld\n", c = a + b);
        else if (ch == '-')
            printf("%ld\n", c = a - b);
        else if (ch == '*')
            printf("%ld\n", c = a * b);
        else
        {
            if (a % b)
                printf("%.2lf\n", d = (double)a/(double)b);
            else
                printf("%ld\n", c = a / b);
            // 结果不是整数才保留2位
        }
    }
}