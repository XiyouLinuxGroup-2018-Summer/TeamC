/**
* Untitled-1
* > author: PhoenixXC
* > email: xuancbm@xiyoulinux.org
* > description: D - 二进制
  >     暴力BFS
  >     初始节点设为1，把节点放入空队列
        如果队列非空
            每个节点i都和 i*10 和 i*10+1 相关
            如果某个节点不满足%n == 0, 此节点出列，把与他相关的节点放入队列
            如果满足，输出节点，然后return退出
            在队列里面继续寻找
* > created: Wed Aug 01 2018 08:02:27 GMT+0800 (CST)
* > last-modified: Wed Aug 01 2018 08:02:27 GMT+0800 (CST)
*/

#include <stdio.h>
#include <string.h>
#include <queue>
using namespace std;

queue<long long>q;
long long n;
void BFS()
{
    while(!q.empty())
    {
        long long temp = q.front();
        if (temp % n == 0)
        {
            printf("%lld\n", temp);
            return;
        }
        else
        {
            q.pop();
            q.push(temp * 10 + 1);
            q.push(temp * 10);
        }
    }
}

int main()
{
    while (scanf("%lld", &n) == 1 && n)
    {
        while (!q.empty())          // 清空队列
            q.pop();

        q.push(1);
        BFS();                     // 1 为初始点
    }
}
