#include <stdio.h>

int main()
{
    int n, m;
    int a[5];
    while(scanf("%d  %d", &n, &m) != EOF)
    {
	if(n == 0 && m == 0)
	{
	    break;
	}
        int i, j, k;
	for(i = 0; i < n; i++)
	{
	    scanf("%d", &a[i]);
	}
	a[i] = m;
	for(j = 0; j < n + 1; j++)
	{
	    for(k = 0; k < n + 1 - j - 1; k++)
	    {
	        if(a[k] > a[k + 1])
		{
		    int tmp = a[k];
		    a[k] = a[k + 1];
		    a[k + 1] = tmp;
		}
	    }
	}
	for(j = 0; j < n + 1; j++)
	{
	    printf("%d  ", a[j]);
	}
	printf("\n");
    }
    return 0;
}
