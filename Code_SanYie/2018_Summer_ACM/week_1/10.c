#include<stdio.h>

int s(int x)
{
    int i, sum = 0;
    for(i = 1; i < x; i++)
    {
        if(x%i == 0)
        {
            sum += i;
        }
    }
    return sum;
}
int main()
{
    int n, a, b;
    scanf("%d", &n);
    while(n--)
    {
        scanf("%d %d", &a, &b);
        int s1 = s(a);
        int s2 = s(b);
        if(s1 == b && s2 == a)
            printf("YES\n");
        else
            printf("NO\n");
    }

}