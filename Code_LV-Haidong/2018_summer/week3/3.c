#include<stdio.h>
#include<stdlib.h>
#include<string.h>
int main()
{
    int n,i,j;
    int a[1000];
    int dp[1000],sum=0;
    while(scanf("%d",&n)&&n)
    {
         memset(a,0,sizeof(a));
         memset(dp,0,sizeof(dp));
          for(i=0;i<n;i++)
          {
              scanf("%d",&a[i]);//输入内容
          }
          dp[0]=1;
         for(i=1;i<=n;i++)
         {
             sum=0;
             if(a[i]>a[n])
             continue;
            for(j=0;j<i;j++)
            {       
                if(a[i]>a[j])
                {
                   if(sum<dp[j])
                   sum=dp[j];
                }
            }
            dp[i]=sum+1;
         }
         sum=0;
         for(i=0;i<=n;i++)
         {
             if(sum<dp[i])
             sum=dp[i];
         }
         printf("%d\n",sum);
    }
    return 0;
}