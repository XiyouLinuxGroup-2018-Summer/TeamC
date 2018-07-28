#include <stdio.h>
#include <string.h>

int main()
{
    char a[50];
    while(gets(a) != EOF)
    {
        int b = strlen(a);
	int i;
	a[0] = toupper(a[0]);
	for(i = 1; i < b; i++)
	{
	    if(a[i] == ' ')
	    {
	        a[i + 1] = toupper(a[i + 1]);   
	    }
	}
	puts(a);
	printf("\n");
    }
    return 0;
}
