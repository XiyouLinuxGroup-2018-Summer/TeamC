/**
* 10.c
* @author: PhoenixXC
* @email: xuancbm@xiyoulinux.org
* @description: J - 亲和数 
* @created: Tue Jul 24 2018 09:10:06 GMT+0800 (CST)
* @last-modified: Tue Jul 24 2018 09:10:06 GMT+0800 (CST)
*/

#include <stdio.h>
int sum(int a)
{
    int all = 0, i;
    for (i = 2; i * i < a; i++)
    {
        if (a % i == 0)
            all += (i + a / i);
    }
    if (i != 1)
        all++;
    if (i * i == a)
        all += a;
    return all;
} 

int main(void)
{
    int M;
    scanf("%d", &M);
    for (int i = 0; i < M; i++)
    {
        int A, B;
        scanf("%d %d", &A, &B);
        if (sum(A) == B && sum(B) == A)
            printf("YES\n");
        else
            printf("NO\n");
    }
    return 0;
}