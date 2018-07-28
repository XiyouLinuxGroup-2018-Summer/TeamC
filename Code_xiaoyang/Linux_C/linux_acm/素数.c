#include <stdio.h>
#define f(x) x * x + x + 41

int main()
{
    int x, y;
    while(scanf("%d %d", &x, &y) != EOF)
    {
        if(x == 0 && y == 0)
	{
	    break;
	}
	int i, m, j, flag = 0;
	for(i = x; i <= y; i++)
	{
	    m = f(i);
	    for(j = 2; j < m / 2; j++)
	    {
	        if(m % j == 0)
		{
		    flag = 1;
		    break;
		}
	    }
	}
	if(flag == 0)
	{
	    printf("OK\n");
	}
	else
	{
	    printf("Sorry\n");
	}
    }
    return 0;
}
