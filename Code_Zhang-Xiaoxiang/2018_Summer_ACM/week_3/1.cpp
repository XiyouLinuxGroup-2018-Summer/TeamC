/**
* 1.cpp
* > author: PhoenixXC
* > email: xuancbm@xiyoulinux.org
* > description: A - 数字之塔
    思路：
        可求得动态转移方程：acc[i][j] = num[i][j] + max(acc[i+1][j], acc[i+1][j+1])
        然后记住已经算过的以节点为起点的最大和，逐步递归求解
* > created: Wed Aug 08 2018 22:05:14 GMT+0800 (CST)
* > last-modified: Wed Aug 08 2018 22:05:14 GMT+0800 (CST)
*/



#include <stdio.h>
#include <string.h>
#include <algorithm>
using namespace std;
#define MAXN 100
static int tree[MAXN][MAXN], H;
static int node[MAXN][MAXN];

int acc(int i, int j)
{
    if (i == H - 1)
        return tree[i][j];
    else if (node[i][j] < 0)
        return node[i][j] = (tree[i][j] + max(acc(i+1, j), acc(i+1, j+1)));
    else 
        return node[i][j];
}

int main(void)
{
    int T;
    scanf("%d", &T);
    for(int i = 0; i < T; i++)
    {
        scanf("%d", &H);
        memset(tree, 0, sizeof(tree));
        memset(node, -1, sizeof(node));
        for (int m = 0; m < H; m++)
            for (int n  = 0; n <= m; n++)
                scanf("%d", &tree[m][n]);
        printf("%d\n", acc(0, 0));
    }
}
