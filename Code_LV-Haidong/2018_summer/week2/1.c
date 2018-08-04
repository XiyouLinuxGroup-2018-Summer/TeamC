#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <set>
#include <queue>
#include <stack>
 
using namespace std;
char map[10][10];
int vis[10];//第i列是否放置了棋子
int cnt;//已放棋子的数目
int sum;//放置方法的总数
int n,k;
void dfs(int s)
{
    int i;
    if(cnt==k){//所有的棋子都放置好
        sum++;
        return ;
    }
    else{
        if(s>=n)//如果越界
            return ;//返回
        else{
            for(i=0;i<n;i++){//讲一个棋子尝试放在0-n-1列的某一行
                if(map[s][i]=='#'&&!vis[i]){
                    vis[i]=1;//标记该列已经放了棋子
                    cnt++;//棋子数+1
                    dfs(s+1);//继续搜索
                    cnt--;//经过一轮递归后num始终保持不变,因为没有放棋子
                    vis[i]=0;//在此处不放棋子
                }
            }
            dfs(s+1);//进行剩下的k-1个棋子的遍历
        }
    }
}
int main()
{
    int i;
    while(~scanf("%d %d",&n,&k)){
            getchar();
        if(n==-1&&k==-1) break;
        memset(vis,0,sizeof(vis));
        for(i=0;i<n;i++)
            scanf("%s",map[i]);
    cnt=sum=0;
        dfs(0);
        printf("%d\n",sum);
    }
    return 0;
}