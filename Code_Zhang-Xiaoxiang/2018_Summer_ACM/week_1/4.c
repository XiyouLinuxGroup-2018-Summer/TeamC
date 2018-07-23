/**
* 4.c
* @author: PhoenixXC
* @email: xuancbm@xiyoulinux.org
* @description: D - 最小公倍数 
* @created: Mon Jul 23 2018 16:05:31 GMT+0800 (CST)
* @last-modified: Mon Jul 23 2018 16:05:31 GMT+0800 (CST)
*/

// 32 45 45/32 == 1...13 32/13 == 2...6 13/6 == 2...1 6/1 == 6...0
#include <stdio.h>
int funmax(int a, int b)
{
    int t, m;
    do
    {
        if (a < b)
        {
            t = a;
            a = b;
            b = t;
        } 
        m = a % b;
        a = b;
        b = m;       
    }while(m);

    return a;
}
int main(void)
{
    int a, b;
    while (scanf("%d %d", &a, &b) == 2)
    {
        int k = a * b / funmax(a, b);
        printf("%d\n", k);
    }
    return 0;
}