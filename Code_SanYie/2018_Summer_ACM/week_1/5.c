#include<stdio.h>

int main()
{
    int n, a, b;
    char op;
    scanf("%d\n", &n);
    while(n--)
    {
        scanf("%c %d %d", &op, &a, &b);
        if(op == '+')
        {
            printf("%d\n", a + b);
        }
        else if(op == '-')
        {
            printf("%d\n", a - b);
        }
        else if(op == '*')
        {
            printf("%d\n", a * b);
        }
        else if(op == '/')
        {
            if(a % b != 0)
                printf("%.2lf\n", (double)a/b);
            else
                printf("%d\n", a / b);
        }
        getchar();
    }
}