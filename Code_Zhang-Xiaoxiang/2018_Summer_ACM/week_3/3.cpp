/**
* 3.cpp
* > author: PhoenixXC
* > email: xuancbm@xiyoulinux.org
* > description: C - 超级跳跃
    思路:
    以起点和终点作为最开始和最后的点，设终点的值为0
    状态转移方程为:
        max(i) = num(i) + max{max(a), max(b), ... , max(z)} (当i不是终点时需要 num(a),num(b),..., num(z) > num(i))
        (z < .... < b < a < i)
* > created: Tue Aug 07 2018 17:16:25 GMT+0800 (CST)
* > last-modified: Tue Aug 07 2018 17:16:25 GMT+0800 (CST)
*/

#include <bits/stdc++.h>
using namespace std;

#define maxn 1005
int sum[maxn];
int d[maxn], N;

int dp(int k)
{
    if (k == 0)
        return 0;
    if (sum[k] >= 0)
        return sum[k];
    sum[k] = 0;
    for (int i = k - 1; i >= 0; i--)
    {
        if (d[i] <= 0 || ((k != N + 1) && d[i] >= d[k]))
            continue;
        sum[k] = max(sum[k], dp(i));
    }
    return sum[k] += d[k];
}

int main()
{
    while (cin >> N && N)
    {
        for (int i = 1; i <= N; i++)
            cin >> d[i];
        d[0] = d[N + 1] = 0;
        memset(sum, -1, sizeof(sum));
        cout << dp(N + 1) << endl;
    }
}
