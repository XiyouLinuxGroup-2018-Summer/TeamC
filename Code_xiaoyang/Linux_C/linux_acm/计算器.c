#include <stdio.h>

float cal()
{
    char a[10];
    scanf("%s", a);
    switch(a[0])
    {
        case '+':
		return cal() + cal();
	case '-':
		return cal() - cal();
	case '*':
		return cal() * cal();
	case '/':
		return cal() / cal();
	default:
                return atof(a);
		break;
    }
}

int main()
{
    int n, i;
    scanf("%d", &n);
    for(i = 0; i < n; i++)
    {
        printf("%f\n", cal());
    }
}
