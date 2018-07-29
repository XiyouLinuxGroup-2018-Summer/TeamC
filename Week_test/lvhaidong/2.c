#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
int convert(const char * num)
{
    int i=0;
    int j=0;
    int k=0;
    for(j=strlen(num)-1;j>=0&&k<(int)strlen(num);j--)
    {
            i+=(num[j]-'0')*pow(10,k);
            k++;
    }
    return  i; 
}
int main()
{
    char *num="123456";
    int i=0;
    i=convert(num);
    printf("%d",i);
    return 0;
}
