#include<stdio.h>
#include<stdlib.h>
#include<string.h>
int fun(int a[],int n)
{
    if(n==1)
    return 1;
    int dp[1005];
    int i=0,j=1,sum=0;
    memset(dp,0,sizeof(dp));
    dp[0]=1;
       for(i=1;i<=n;i++)
      {
          sum=0;
          for(j=0;j<i;j++)
         {
               if(a[i]>a[j])
               {
                   if(sum<dp[j])
                   sum = dp[j];
               }   
         }
         dp[i]=sum+1;
      }
      sum=0;
    for(i=1;i<=n;i++)
    {
         if(sum<dp[i])
         sum=dp[i];
    }
        return sum;
}
int main()
{
    int n,i;
    int a[1005];
    while(scanf("%d",&n)!=EOF)
    {
      memset(a,0,sizeof(a));
      for(i=0;i<n;i++)
       scanf("%d",&a[i]);
       printf("%d\n",fun(a,n));
    }
    return 0;
}