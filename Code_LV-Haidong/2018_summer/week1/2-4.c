#include<stdio.h>
#include<stdlib.h>
#include<string.h>
int main()
{
    int a[10],b[10],m,n,c,t;
    int i=0,j=0;
    while(scanf("%d %d",&a[i],&b[i])!=EOF)
    {
        i++;
    }
    for(j=0;j<i;j++)
    {
        m=a[j];
       n=b[j];
     if(a[j]<b[j])
    {
         t=a[j];
         a[j]=b[j];
         b[j]=t;
    }
    while(b[j]!=0)
    {
        c=a[j]%b[j];
        a[j]=b[j];
        b[j]=c;
    }
    printf("%d\n",m*n/a[j]);
    }
    return 0;
}
