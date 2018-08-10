/**
* 4.cpp
* > author: PhoenixXC
* > email: xuancbm@xiyoulinux.org
* > description: 
    思路：
        与上一题相似，只是这题要求最长升序路
        动态转移方程为 max(n) = 1 + max{max(a), max(b), max(c) .... max(z)};
           需要满足 ((a, b, c, ...., z < n) && num(n) > (num(a), num(b), ...num(z)) )
        为了方便设置节点的值为max(int),即1<<31-1, 然后递归+记忆化搜索求解即可
* > created: Tue Aug 07 2018 17:37:42 GMT+0800 (CST)
* > last-modified: Tue Aug 07 2018 17:37:42 GMT+0800 (CST)
*/

#include <iostream>
#include <cstring>
#include <cmath>
#include <algorithm>
using namespace std;
#define maxn 1005

int num[maxn], a[maxn];
int N;

int dp(int k)
{
    if (k == 0)
        return 0;
    if (num[k] >= 0)
        return num[k];
    num[k] = 0;
    for (int i = k - 1; i >= 0; i--)
    {
        if (a[i] >= a[k])
            continue;
        num[k] = max(num[k], dp(i));
    }
    return num[k] += 1;
}

int main()
{
    while (cin >> N)
    {
        for (int i = 1; i <= N; i++)
            cin >> a[i];
        a[N + 1] = 1 << 31 - 1;
        memset(num, -1, sizeof(num));
        cout << dp(N + 1) - 1 << endl;
    }
}
