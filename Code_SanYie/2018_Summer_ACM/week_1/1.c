#include<stdio.h>

int main()
{
    char arr[4];
    int i, j;
    while(scanf("%s", arr) != EOF)
    {
        for(i = 0; i < 2; i++)
        {
            for(j = i + 1; j < 3; j++)
            {
                if(arr[i] > arr[j])
                {
                    char temp = arr[i];
                    arr[i] = arr[j];
                    arr[j] = temp;
                }
            }
        }
        printf("%c %c %c\n", arr[0], arr[1], arr[2]);
    }
}