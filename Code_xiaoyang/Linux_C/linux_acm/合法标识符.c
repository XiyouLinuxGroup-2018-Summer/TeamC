#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main()
{
    int n, i, j;
    char a[50];
    scanf("%d", &n);
    for(i = 0; i < n; i++)
    {
	getchar();
        gets(a);
	int b = strlen(a);
	if(a[0] >= 47 && a[0] <= 57)
	{
	    printf("no\n");
	    continue;
	}
        for(j = 1; j < b; j++)
	{
	    if(a[j] == ' ')
	    {
	        printf("no\n");
		break;
	    }
	}
	if(j == b)
	{
	    printf("yes\n");
	}
    }
    return 0;
}
