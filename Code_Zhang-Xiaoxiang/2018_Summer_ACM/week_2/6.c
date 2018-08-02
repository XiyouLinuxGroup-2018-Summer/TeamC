/**
* 6.c
* > author: PhoenixXC
* > email: xuancbm@xiyoulinux.org
* > description: F - 勘探石油 
    输入后，从第一行开始查找，如果查找到"@", 就开始dfs寻找与他相连的所有油田并标记，继续寻找，如果查找到的油田没有被标记过，开始dfs
    第一行搜完，逐层查找下一行，直到查找完
* > created: Wed Aug 01 2018 08:31:08 GMT+0800 (CST)
* > last-modified: Wed Aug 01 2018 08:31:08 GMT+0800 (CST)
*/

#include <stdio.h>
#include <string.h>
#define MAXN 105

int vst[MAXN][MAXN];    // 记录是否被查找过
char map[MAXN][MAXN];   // 地图
int count;              // 区域数量
int M, N;

int CheckIsEdge(int x, int y)       // 检查是否满足条件，未超出范围且是没有被标记过的'@'
{
    if ((x >= 0 && x < M && y >= 0 && y < N) && map[x][y] == '@' && vst[x][y] == 0)
        return 1;
    else
        return 0;
}

void DFS(int x, int y)              // 递归遍历四周所有的油田并标记
{
    vst[x][y] = 1;
    for (int dx = -1; dx <= 1; dx++)
    {
        for (int dy = -1; dy <= 1; dy++)
        {
            if (dx == 0 && dy == 0)
                continue;
            if (CheckIsEdge(x + dx, y + dy))
                DFS(x + dx, y + dy);
        }
    }
    return;
}

int main(void)
{
    while (scanf("%d %d", &M, &N) == 2 && M && N)
    {
        while(getchar() != '\n') ;
        count = 0;
        memset(vst, 0, sizeof(vst));

        for (int i = 0; i < M; i++)
        {
            for (int j = 0; j < N; j++)
                scanf("%c", &map[i][j]);
            getchar();
        }
        for (int i = 0; i < M; i++)     // 从数组的第i行第j列一个一个找
        {
            for (int j = 0; j < N; j++)
            {
                if (map[i][j] == '@' && vst[i][j] == 0)
                {
                    count++;
                    DFS(i, j);
                }
            }
        }
        printf("%d\n", count);
    }
}
