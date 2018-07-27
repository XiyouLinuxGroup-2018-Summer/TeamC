#include<stdio.h>
#include<string.h>

int main()
{
    int n, i;
    char a[100];
    scanf("%d", &n);
    while(n--)
    {
        scanf("%s   ", a);
        int len = strlen(a), t = 0;
        for(i = 0; i < len; i++)
        {
            if(a[i] >= '0' && a[i] <= '9')
                t++;
        }
        printf("%d\n", t);
    }
}