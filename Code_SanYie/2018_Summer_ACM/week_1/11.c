#include<stdio.h>

int isprimer(int n)
{
    int t = n / 2, i;
    for(i = 2; i < t; i++)
    {
        if(n % i == 0)
            return 0;
    }
    return 1;
}
int main()
{
    int x,y;
    while(scanf("%d %d", &x, &y))
    {
        int f = 1;
        if(x == 0 && y ==0)
            break;
        int i = x;
        for(i = x; i <= y; i++)
        {
            int s = i*i + i + 41;
            if(!isprimer(s))
            {
                f = 0;
                printf("Sorry\n");  
                break;
            } 
        }
        if(f)
        {
            printf("OK\n");
        }
    }
}