#include<stdio.h>
#include<string.h>

int main()
{
    int i, j, n, f;
    char a[100];
    scanf("%d", &n);
    while(n--)
    {
        scanf("%s", a);
        int len = strlen(a);
        f = i = 0;
        j = len -1;
        while(i < j)
        {
            if(a[i] == a[j])
            {
                i++;
                j--;
            }
            else
            {
                printf("no\n");
                f = 1;
                break;
            }
        }
        if(!f)
            printf("yes\n");
    }
}