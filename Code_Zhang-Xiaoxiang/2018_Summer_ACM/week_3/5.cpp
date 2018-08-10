/**
* 5.cpp
* > author: PhoenixXC
* > email: xuancbm@xiyoulinux.org
* > description: 
    思路：
        和前面的几道依然很类似，只是从一维变到二维，限制了跳跃范围。
        读入矩阵后，以(0,0)为起点开始dfs, 当数组范围未越界且搜索区域的值大于当前区域便继续搜索。
        因为是要求升序，所以不需要标记走过的路
        动态转移方程为:
            max[i][j] = num[i][j] + max{max[a][b],...max(四周可以搜索的点的坐标)}
        然后递归＋记忆化求解
        emmm效率好像不是很好
* > created: Thu Aug 09 2018 10:12:29 GMT+0800 (CST)
* > last-modified: Thu Aug 09 2018 10:12:29 GMT+0800 (CST)
*/

#include <iostream>
#include <algorithm>
#include <cstring>
#include <cmath>
using namespace std;

#define maxn 105
int map[maxn][maxn];
int sum[maxn][maxn];
int N, M;

bool isJump(int i, int j, int value)            // 判断边界
{
    if (i >= N || i < 0 || j >= N || j < 0 || map[i][j] <= value)
        return 0;
    return 1;
}

int dp(int i, int j)
{
    if (sum[i][j] >= 0)
        return sum[i][j];
    sum[i][j] = 0;
    for (int k = -M; k <= M; k++)              
    {
        for (int l = -M; l <= M; l++)
        {
            if (k == 0 && l == 0)
                continue;
            if (k != 0 && l != 0)
                continue;
            if (isJump(i + k, j + l, map[i][j]))
                sum[i][j] = max(sum[i][j], dp(i + k, j + l));
        }
    } 
    return (sum[i][j] += map[i][j]);
}

int main()
{
    while (cin >> N >> M && !(N == -1 && M == -1))
    {
        for (int i = 0; i < N; i++)
            for (int j = 0; j < N; j++)
                cin >> map[i][j];
        memset(sum, -1, sizeof(sum));
        cout << dp(0, 0) << endl;
    }
}
