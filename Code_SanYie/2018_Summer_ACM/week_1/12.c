#include<stdio.h>
#include<string.h>

int main()
{
    char a[100];
    while(scanf("%[^\n]", a) != EOF)
    {
        getchar();
        char* t = a;
        a[0] = a[0] - 32;
        while(t != NULL)
        {
            t = strstr(t, " ");
            if(t)
            {
                t++;
                *t = *t -32;
            }
        }
        printf("%s\n", a);
    }
}