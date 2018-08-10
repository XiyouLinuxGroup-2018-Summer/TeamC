#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <algorithm>
using namespace std;
const int INF=0x3f3f3f3f;
int dp[2005][2];
int x[2005],y[2005];
int main(){
    int t,i,j,n,a,b,c,ans;
    scanf("%d",&t);
    while(t--){
        scanf("%d",&n);
        for(i=1;i<=n;i++)
        scanf("%d",&x[i]);
        for(i=2;i<=n;i++)
        scanf("%d",&y[i]);
        memset(dp,INF,sizeof(dp));
        dp[1][0]=dp[1][1]=x[1];                 //dp[i][0]表示第i个人单独买票
        for(i=2;i<=n;i++){                      //dp[i][1]表示dii个人和前面的人一起买票
            dp[i][1]=dp[i-1][0]+y[i]-x[i-1];
            dp[i][0]=min(dp[i-1][0],dp[i-1][1])+x[i];
        }
        ans=min(dp[n][0],dp[n][1]);
        a=8,b=c=0;                              //时间转换
        /*b+=(ans/60);
        c=ans%60;
        a+=(b/60);
        b%=60;*/
        c=(ans%3600)%60;
        b=ans%3600/60;
        a+=ans/3600;
        if(a>12||(a==12&&(b>0||c>0)))
        printf("%.2d:%.2d:%.2d pm\n",a,b,c);
        else
        printf("%.2d:%.2d:%.2d am\n",a,b,c);
    }
    return 0;
}