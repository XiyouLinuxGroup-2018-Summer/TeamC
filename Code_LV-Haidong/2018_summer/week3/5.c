#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int N, M ;
int a[105][105];
int dp[105][105];
int next[4][2] = { 1, 0, -1, 0, 0, 1, 0, -1};
int dfs(int x, int y)
{
   if (dp[x][y])
        return dp[x][y];
    int i, tx, ty, k;
    int dp1 = 0;
    for (i = 1; i <= M; i++)
    {
        for (k = 0; k < 4; k++)
        {
            tx = x + next[k][0] * i;
            ty = y + next[k][1] * i;
            if (tx < 1 || tx > N || ty < 1 || ty > N)
                continue;
            if (a[tx][ty] > a[x][y])
            {
                int tmp = dfs(tx,ty);
                if (dp1 < tmp)
                    dp1 = tmp ;
            }
        }
    }
    dp[x][y] = dp1 + a[x][y];
    return dp[x][y];
}
int main()
{
    int i, j;
    while (scanf("%d %d", &N, &M) != EOF)
    {
        memset(dp, 0, sizeof(dp));
        memset(a, 0, sizeof(a));
        if (M == -1 && N == -1)
            break;
        for (j = 1; j <= N; j++)
            for (i = 1; i <= N; i++)
                scanf("%d", &a[j][i]);
        printf("%d\n",dfs(1, 1));
    }
    return 0;
}