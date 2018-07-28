#include<stdio.h>
#include<string.h>

int check(char* a)
{   
    if(a[0] >= '0' && a[0] <= '9')
        return 0;
    int len = strlen(a), i;
    for(i = 0; i < len; i++)
    {
        if(!(a[i]>='0'&&a[i]<='9'||a[i]>='a'&&a[i]<='z'||a[i]>='A'&&a[i]<='Z'||a[i]=='_'))
            return 0;
    }
    return 1;
}
int main()
{
    int n;
    char a[100];
    scanf("%d", &n);    
    while(n--)
    {
        getchar();
        scanf("%[^\n]", a);
        if(check(a))
        {
            printf("yes\n");
        }
        else
        {
            printf("no\n");
        }
    }
}