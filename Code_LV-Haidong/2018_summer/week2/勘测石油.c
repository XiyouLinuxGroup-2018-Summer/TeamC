//这道题的思路就是通过dfs算法，来遍历石油的上下左右，有没有相同的
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
char a[105][105];
int  b[105][105];
int M,N,sum=0;//采矿机的数量
void dfs(int i,int j)//将他的四周进行遍历
{
       if ((a[i][j] != '@')||b[i][j]==1) //越界和非石油的地方3 5
        return;
    b[i][j]=1;
      dfs(i-1,j-1);
    dfs(i-1,j);
    dfs(i-1,j+1);
    dfs(i,j-1);
    dfs(i,j+1);
    dfs(i+1,j-1);
    dfs(i+1,j);
    dfs(i+1,j+1);
}
int main()
{
    int i,j;
    scanf("%d %d",&M,&N);
    while((M!=0)&&(N!=0))
    {
        memset(b,0,sizeof(b));
        memset(a,0,sizeof(a));
        getchar();
       for(i=1;i<=M;i++)
        {
             for(j=1;j<=N;j++)
         {
             scanf("%c",&a[i][j]);
         }
         getchar();/////使用%c必须要接收回车
        }
         sum=0;
         for(i=1;i<=M;i++)
          for(j=1;j<=N;j++)
          {
              if(a[i][j]=='@'&&b[i][j]==0)
              {
                dfs(i,j);
                sum++;
               }
          }
          printf("%d\n",sum);
       scanf("%d %d",&M,&N);
    }
    return 0;
}
