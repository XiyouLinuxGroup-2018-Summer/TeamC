#include<stdio.h>

void swap(int* a, int *b)
{
    int t = *a;
    *a = *b;
    *b = t;
}
int main()
{
    int a[2], i;
    while(scanf("%d %d", &a[0], &a[1]) != EOF)
    {
        if(a[0] > a[1])
        {
            swap(&a[0], &a[1]);
        }
        int m = a[0] * a[1];
        for(i = a[1]; i <= m; i++)
        {
            if(i % a[0] == 0 && i % a[1] ==0)
            {
                printf("%d\n", i);
                break;
            }
        }
    }
}