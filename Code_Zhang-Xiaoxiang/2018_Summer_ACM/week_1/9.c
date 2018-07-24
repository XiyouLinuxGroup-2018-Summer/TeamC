/**
* 9.c
* @author: PhoenixXC
* @email: xuancbm@xiyoulinux.org
* @description: I - 三角形
* @created: Mon Jul 23 2018 19:28:15 GMT+0800 (CST)
* @last-modified: Mon Jul 23 2018 19:28:15 GMT+0800 (CST)
*/

#include <stdio.h>
int main(void)
{
    int N;
    scanf("%d", &N);
    for (int i = 0; i < N; i++)
    {
        float a, b, c;
        scanf("%f %f %f", &a, &b, &c);
        if ((b + c > a) && (a + c > b) && (a + b > c))
            printf("YES\n");
        else
            printf("NO\n");
    }
    return 0;
}