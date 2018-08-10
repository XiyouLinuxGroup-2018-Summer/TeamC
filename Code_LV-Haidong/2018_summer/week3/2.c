#include<stdio.h>
#include<string.h>
int main()
{
    int n,i=1,j;
    int count[32876];
    while(scanf("%d",&n)==1)
    {
        memset(count,0,sizeof(count));
        count[0]=1;
        i=1;
       while(i<=3)
       {
             for(j=i;j<=n;j++)
         {
             count[j]+=count[j-i];
         }
         i++;
       }
       printf("%d\n",count[n]);
    }
}