/**
* 1.c
* > author: PhoenixXC
* > email: xuancbm@xiyoulinux.org
* > description: A - 5×5迷宫 
* > created: Mon Jul 30 2018 17:15:32 GMT+0800 (CST)
* > last-modified: Mon Jul 30 2018 17:15:32 GMT+0800 (CST)
*/

#include <stdio.h>
#include <string.h>
#define MAXN 5

int vst[MAXN][MAXN];     // 访问标记，记录是否走过
int map[MAXN][MAXN];     // 坐标范围
int depth = 0;           // 路长
int mindh = MAXN * MAXN;
int path[MAXN * MAXN][MAXN * MAXN];
int min;
int count = 0;
int dir[4][2] = 
{
    {0, 1}, {0, -1}, {1, 0}, {-1, 0}    // 右 左 上 下
};

int CheckEdge(int x, int y)            // 边界、约束判断      
{
    // 如果节点没走过且没有越界
    if ((x >= 0 && x < MAXN && y >= 0 && y < MAXN) && !map[x][y] && !vst[x][y])
        return 1;
    else
        return 0;
}

void dfs(int x, int y)
{
    path[count][depth] = x*10 + y;
    depth++;
    vst[x][y] = 1;  // 标记访问过的节点
    
    // printf("(%d, %d) depth = %d\n", x, y, depth);
    if (x == MAXN - 1 && y == MAXN - 1)
    {
        if (depth < mindh)
        {                         
            mindh = depth;
            min = count;
            count++;
            memcpy(path[count], path[count-1], sizeof(path[count-1]));
        }
        // printf("\n");
        // printf("depth = %d\n\n", depth);
        return;
    }

    for (int i = 0; i < 4; i++)
    {
        if (CheckEdge(x + dir[i][0], y + dir[i][1]))
        {
            dfs(x + dir[i][0], y + dir[i][1]);
            vst[x + dir[i][0]][y + dir[i][1]] = 0;
            depth--;                                // 走另一条路，路线减一
        }
    }
    // printf("\n");
    return;
}

int main(void)
{
    int x = 0, y = 0;
    for (int i = 0; i < MAXN; i++)
        for (int j = 0; j < MAXN; j++)
            scanf("%d", &map[i][j]);
    dfs(x, y);
    // printf("min step is %d\n", mindh);
    for (int i = 0; i < mindh; i++)
    {
        printf("(%d, %d)\n", path[min][i]/10, path[min][i]%10);
    }
    return 0;
}