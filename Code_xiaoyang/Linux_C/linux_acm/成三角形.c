#include <stdio.h>

int main()
{
    int n, a, b, c, i;
    scanf("%d", &n);
    for(i = 0; i < n; i++)
    {
       scanf("%d %d %d", &a, &b, &c);
       if(a + b <= c)
       {
           printf("NO\n");
       }
       else if(a + c <= b)
       {
           printf("NO\n");
       }
       else if(b + c <= a)
       {
           printf("NO\n");
       }
       else
       {
           printf("YES\n");
       }
    }
    return 0;
}
