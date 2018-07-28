#include <stdio.h>

void swap(char * a, char * b)
{
    char tmp = *a;
    *a = *b;
    *b = tmp;
}

int main()
{
    char a,b,c;
    while(scanf("%c%c%c",&a,&b,&c) != EOF)
    {
        if(a > b)
	{
	    swap(&a, &b);
	}
	if(b > c)
	{
	    swap(&b, &c);
	}
	if(a > c)
	{
	    swap(&a, &c);
	}

	printf("%c %c %c\n",a,b,c);
    }
    return 0;
}
