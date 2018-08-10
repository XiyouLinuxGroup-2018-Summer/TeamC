/**
* 2.cpp
* > author: PhoenixXC
* > email: xuancbm@xiyoulinux.org
* > description: B - 兑换硬币
    思路:
        没有使用动态规划。。。
        对于一个数值n
        1. 先考虑全为１的情况，仅１种，即 1+1+......+1+1 == n
        2. 在考虑仅有1和2的情况，这种情况的解等价于　x*2 + y*1 == n (x != 0)　的解的个数，可求得x有n/2种可能
        3. 接着考虑有3的情况，上面两种情况是无3的情况，有3的情况可以细分为仅有1,2,3....n/3 个3的情况，假设有m个3,
        　　那么n - m*3这个数必然由1和2组成，也就是回到了上面两种情况，
* > created: Mon Aug 06 2018 16:38:24 GMT+0800 (CST)
* > last-modified: Mon Aug 06 2018 16:38:24 GMT+0800 (CST)
*/

#include <bits/stdc++.h>
using namespace std;

int main()
{
    int N;
    while(cin >> N)
    {
        long long sum = 1;      // 全为１
        sum += N/2;             // 只考虑1 和 2
        int k = N/3;
        for (int i = 1; i <= k; i++)
        {                       // 有i个３，考虑N - i*3中１，２的组合
            int n = N - i*3;
            sum += 1 + n/2;
        }
        cout << sum << endl;
    }
}
