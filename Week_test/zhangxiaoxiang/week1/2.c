#include <stdio.h>
#include <string.h>

int convert(const char * num)
{
    int flag = 1, i = 0, ans = 0, j = strlen(num) - 1;
    if (num[0] == '-')
    {    
        flag = -1;
        i = 1;
    }
    if (num[0] == '+')
        i = 1;

    for ( ; i <= j; i++)
    {
        ans *= 10;
        ans += (num[i] - '0');
    }
    return flag*ans;
}

int main(void)
{
    char str[9];
    scanf("%s", str);
    printf("the num is %d\n", convert(str));
    return 0;
}