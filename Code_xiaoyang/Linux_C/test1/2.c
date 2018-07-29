#include <stdio.h>
#include <string.h>

int main()
{
    char a[10];
    char d;
    int b, i;
    long c;

    scanf("%s", a);
    b = strlen(a);
    c = a[0] - '0';

    for(i = 1; i < b; i++)
    {
        d = a[i];
        c = c * 10 + (d - '0');    	
    }
    printf("%d\n", c);
    return 0;
}
