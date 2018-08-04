//类似与八皇后问题，应该使用dfs算法进行遍历
#include <stdio.h>
#include<stdlib.h>
#include <string.h>
char chess[8][8];
int chess1[8];
int sum,a,cnt=0,b;
void dfs(int s)
{
       int i;
       
       if(cnt==b)
       {
           sum++;
           return;
       }       
       else{
           if(s>=a)//越界
           return ;
           else
           {
               for(i=0;i<a;i++)//尝试把棋子放入
               {
                   if((chess[s][i])=='#'&&(!chess1[i]))
                   {
                       chess1[i]=1;
                       cnt++;
                       dfs(s+1);
                       cnt--;
                       chess1[i]=0;
                   }
                }
                 
             }
            dfs(s+1);
         }
}
int main()
{
    int j;
    while(scanf("%d %d",&a,&b)&&(a!=-1)&&(b!=-1))
    { 
        memset(chess,0,sizeof(chess));
        memset(chess1,0,sizeof(chess1));
        for(j=0;j<a;j++)
         scanf("%s",chess[j]);
         sum=cnt=0;
         dfs(0);
         printf("%d\n",sum);
    }
    return 0;
}
