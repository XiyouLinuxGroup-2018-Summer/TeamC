#include <iostream>
#include <cstdio>
#include <cstring>
#define MM(a,b) a<b?b:a
using namespace std;
int n,m,map[111][111],sum[111][111],dd[4][2]={0,1,0,-1,1,0,-1,0};
int dfs(int X,int Y)
{
    int i,j,k,l=0,x,y;
    if(sum[X][Y])return sum[X][Y];
    for(i=0;i<4;i++)//遍历四个方向
    {
        for(j=1;j<=m;j++)//遍历各个距离1-k
        {
            x=X+dd[i][0]*j;//取点
            y=Y+dd[i][1]*j;
            if(x>=0&&y>=0&&x<n&&y<n)//判断越界
            {
                if(map[x][y]>map[X][Y])//下一个点必须比当前点的数值大
                {
                    k=dfs(x,y);//k记录下一个节点（x，y）的最大值
                    l=MM(l,k);//挑选最大
                    printf("l %d\n",l);
                }
            }
        }
    }
    sum[X][Y]=l+map[X][Y];//可以走各个点的最大值中最大的那个+当前点的数值=当前点的最大值
    return sum[X][Y];//返回当前点的最大值
}
int main (void)
{
    int i,j,k,l;
    while(scanf("%d%d",&n,&m)!=EOF)
    {
        if(n==-1&&m==-1)break;
        for(i=0;i<n;i++)
        {
            for(j=0;j<n;j++)
            {
                scanf("%d",&map[i][j]);
            }
        }
        memset(sum,0,sizeof(sum));
        l=dfs(0,0);
        printf("%d\n",l);
    }
    return 0;
}