/**
 * 3.c
 * > author: PhoenixXC
 * > email: xuancbm@xiyoulinux.org
 * > description:
 *          如果放完了退出，结果加一
 *          如果超了范围就退出，不然死循环
 *          从第d行开始找，如果找到 可以放置棋子且满足条件的地方
 *              放置棋子，找下一行
 *          跳过第d行(第d行不放置棋子或第一行无法放置棋子)
 * > created: Tue Jul 31 2018 11:59:01 GMT+0800 (CST)
 * > last-modified: Tue Jul 31 2018 11:59:01 GMT+0800 (CST)
 */

#include <stdio.h>
#include <string.h>

#define MAXN 8
int  vst[MAXN];        // 标记某列是否有棋子
char map[MAXN][MAXN];  // 记录地图
int  n, k, count;      // 棋盘规格、棋子数、方案数目

void dfs(int d, int p)
{
    if (p == k)        // 如果满足，加一退出
    {
        count++;
        return;
    }
    if (d >= n)
        return;

    for (int i = 0; i < n; i++)      
    {
        if (map[d][i] == '#' && !vst[i])      // 有棋子可走 
        {
            vst[i] = 1;     // 标记此列已有棋子
            dfs(d + 1, p + 1);
            vst[i] = 0;     // 恢复原来的状态
        }                    
    }
    // 跳过此行
    dfs(d + 1, p);

    return;
}

int main(void)
{
    while (scanf("%d %d", &n, &k) == 2 && !(n == -1 && k == -1)) 
    {
        getchar();          
        memset(vst, '\0', sizeof(vst));     // 防止以前的数据的影响
        count = 0;

        for (int i = 0; i < n; i++) 
        {
            for (int j = 0; j < n; j++) 
                scanf("%c", &map[i][j]);
            getchar();
        }
        dfs(0, 0);
        printf("%d\n", count);
    }
}
