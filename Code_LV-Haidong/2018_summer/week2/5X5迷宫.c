//在这里我的思路是深度优先算法
#include<stdio.h>
int a[10][10],b[10][10];
int top,t=25;
int min=99999999;
int x1[50],y2[50];
int x2[50],y3[50];
//在这里n,m表示迷宫的行和列,p,q表示的是终点坐标
int dfs(int x,int y,int step)
{
    int next[4][2]={{0,1},{-1,0},{0,-1},{1,0}};
    int tx,ty,k,q=0;
        if( x==4 && y==4 )
     {
         if(top-1<t)
     for(k=0;k<top;k++)
   {
      x2[k]=x1[k];
      y3[k]=y2[k];
      t=top-1;
    }
        if(step<min)
            min=step;
        return 1;
    }
            for(k=0;k<=3;k++)
      {
        tx=x+next[k][0];//表示走的路径的x坐标
        ty=y+next[k][1];//y坐标
        if(tx <0 || tx>4 || ty>4 || ty<0)
            continue;
        if( a[tx][ty]==0 && b[tx][ty]==0)
        {   
            x1[top]=tx;
            y2[top]=ty;
            top++;
            b[tx][ty]=1;//标记这个点已经走过
           if( dfs(tx,ty,step+1))
                q=1;
               b[tx][ty]=0;
               top--;
        }
      }
    return q;
}
int main()
{
    int i,j;
    for(i=0;i<=4;i++)
        for(j=0;j<=4;j++)
            scanf("%d",&a[i][j]);
    b[0][0]=1;//标记已经走过
    dfs(0,0,0);

    printf("(%d，%d) \n",0,0);

    for(i=0;i<min;i++)
    {
        printf("(%d，%d) \n",x2[i],y3[i]);
    }
        return 0;
}
