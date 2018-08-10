/**
* 6.cpp
* > author: PhoenixXC
* > email: xuancbm@xiyoulinux.org
* > description: F - 超级跳跃 2 
    思路：
    依然是记忆化＋dp
    动态转移方程为: min(n) = max(num(n) + max(n-1), s(n-1) + max(n-2))
                                不与前一个人一起　　　　与前一个人一起
    第一个人只能自己买或和第二个人买
    计算出需要的时间后求出具体时间打印即可
* > created: Wed Aug 08 2018 20:01:06 GMT+0800 (CST)
* > last-modified: Wed Aug 08 2018 20:01:06 GMT+0800 (CST)
*/

#include <iostream>
#include <algorithm>
#include <cstring>
using namespace std;
#define maxn 2005

int T, M;
int num[maxn], ss[maxn], Mem[maxn];

void PrintTime(int sec)    // 根据秒数计算具体时间
{
    int H, M, S, what;
    S = sec % 60;
    H = sec / 3600;
    sec = sec % 3600;
    M = sec / 60;
    H += 8;
    what = H >= 12 ? 1 : 0;
    printf("%02d:%02d:%02d ", H, M, S);
    if (what)
        printf("pm\n");
    else
        printf("am\n");
}

int Dp(int number)
{
    if (number == 0)
        return num[0];
    if (number == 1)
        return num[1];
    if (Mem[number] > 0)
        return Mem[number];
    return Mem[number] = min(num[number] + Dp(number - 1), ss[number - 1] + Dp(number - 2));
}

int main()
{
    cin >> T;
    for (int t = 0; t < T; t++)
    {
        cin >> M;
        for (int i = 1; i <= M; i++)
            cin >> num[i];
        for (int i = 1; i <= M - 1; i++)
            cin >> ss[i];
        memset(Mem, 0, sizeof(Mem));
        ss[0] = num[0] = 0;
        PrintTime(Dp(M));
    }
}
