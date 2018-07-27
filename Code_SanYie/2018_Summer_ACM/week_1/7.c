#include<stdio.h>

void insert(int *a, int n, int x)
{
    int i;
    for(i = n - 1; a[i] > x; i--)
    {
       a[i+1] = a[i];
    }
    a[i + 1] = x;
}
int main()
{
    int a[110];
    int n , x, i, t, f = 1;
    while(scanf("%d %d", &n , &x))
    {
        if(n == 0 && x ==0)
            break;
        for(i = 0; i < n; i++)
        {
            scanf("%d", &a[i]);
        }
        insert(a, n, x);
        for(i = 0; i < n + 1; i++)
        {
            if(i == 0)
                printf("%d", a[i]);
            else
                printf(" %d",a[i]);
        }
        printf("\n");
    }
}