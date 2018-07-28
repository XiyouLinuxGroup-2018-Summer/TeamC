#include <stdio.h>

int yueshu(int x);

int main()
{
    int n, x, y, i, j;
    scanf("%d", &n);
    for(i = 0; i < n; i++)
    {
        scanf("%d %d", &x, &y);
        int sum = yueshu(x);
	if(sum == y)
	{
	    int num = yueshu(y);
	    if(num == x)
	    {
	        printf("YES\n");
	    }
	    else
	    {
	        printf("NO\n");
	    }
	}
	else
	{
	    printf("NO\n");
	}
    }
    return 0;
}

int yueshu(int x)
{
    int i, sum = 0;
    for(i = 1; i < x; i++)
    {
        if(x % i == 0)
	{
	    sum += i;
	}
    }
    return sum;
}

