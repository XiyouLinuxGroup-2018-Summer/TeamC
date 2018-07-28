#include <stdio.h>
#include <string.h>

int main()
{
    int n, i, j, num = 0;
    char a[30];
    scanf("%d", &n);
    for(i = 0; i < n; i++)
    {
	num = 0;
        scanf("%s", a);
	int b = strlen(a);
        for(j = 0; j < b; j++)
	{
	    if(a[j] >= 47 && a[j] <= 57)
	    {
	        num++;
	    }
	}
	printf("%d\n", num);
    }
    return 0;
}
