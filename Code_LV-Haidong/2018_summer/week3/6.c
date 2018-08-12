#include<stdio.h>
#include<stdlib.h>
#include<string.h>
struct time
{
    int hour;
    int mintues;
    int seconds;
};
struct time atime;
int main(void)
{
    int n,N,i,j;
    int firsttime;
    int erverytime[2005],sceondtime[2005],dp[2005];
    scanf("%d",&n);
    while(n--)
    {
        scanf("%d",&N);
        atime.hour=8;
        atime.mintues=0;
        atime.seconds=0;
        for(i=0;i<N;i++)
        {
            scanf("%d",&erverytime[i]);
        }
        for(i=1;i<N;i++)
        {
         scanf("%d",&sceondtime[i]);
        }
        memset(dp,0,sizeof(dp));
        //两种状态，由子问题向多问题求解
        //第i个人单独买票，i-1个人也单独买票
        //第i和i-1个人一起买票
        for(i=0;i<N;i++)
        {
            if(i == 0)
            dp[0]=erverytime[0];
             if(i == 1)
             {
                 if(erverytime[0]+erverytime[1]<sceondtime[1])
                 dp[1]=erverytime[0]+erverytime[1];
                 else
                 dp[1]=sceondtime[1];
             }
             if(i>=2)
             {
                if(dp[i-1]+erverytime[i]<dp[i-2]+sceondtime[i]) 
                 dp[i]=dp[i-1]+erverytime[i];
              else
                 dp[i]=dp[i-2]+sceondtime[i];
             }  
        } 
        sceondtime[N]=dp[i-1];
        atime.seconds=(sceondtime[N]%3600)%60;
        atime.mintues=sceondtime[N]%3600/60;
        atime.hour=8+(sceondtime[N]/3600);
        if(atime.hour < 12)
        {   if(atime.hour<10)
            printf("0");
            printf("%d:",atime.hour);
            if(atime.mintues<10)
            printf("0");
            printf("%d:",atime.mintues);
            if(atime.seconds<10)
            printf("0");
            printf("%d am\n",atime.seconds);
        }
        else
        {
            printf("%d:",atime.hour);
            if(atime.mintues<10)
            printf("0");
            printf("%d:",atime.mintues);
            if(atime.seconds<10)
            printf("0");
            printf("%d pm\n",atime.seconds);
        }  
    }
    return 0;
}