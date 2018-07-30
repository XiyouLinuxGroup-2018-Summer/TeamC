//在这里我的思路是深度优先算法
#include<stdio.h>
int a[10][10],book[10][10];
int o,q,g;
int min=99999999;
int x1[50],y2[50];
//在这里n,m表示迷宫的行和列,p,q表示的是终点坐标
void dfs(int x,int y,int step)
{
    int next[4][2]={{0,1},{-1,0},{0,-1},{1,0}};
    int tx,ty,k;
        if( x==4 && y==4 )
     {
        if(step<min)
            min=step;
        return ;
    }
            for(k=0;k<=3;k++)
      {
        tx=x+next[k][0];//表示走的路径的x坐标
        ty=y+next[k][1];//y坐标
        if(tx <0 || tx>4 || ty>4 || ty<0)
            continue;
        if( a[tx][ty]==0 && book[tx][ty]==0)
        {   
            x1[o]=tx;
            y2[o]=ty;
            o++;
            book[tx][ty]=1;//标记这个点已经走过
            dfs(tx,ty,step+1);
            book[tx][ty]=0;
        }
      }
    return ;
}
int main()
{
    int i,j;
    for(i=0;i<=4;i++)
        for(j=0;j<=4;j++)
            scanf("%d",&a[i][j]);
    book[0][0]=1;//标记已经走过
    dfs(0,0,0);
        return 0;
}
