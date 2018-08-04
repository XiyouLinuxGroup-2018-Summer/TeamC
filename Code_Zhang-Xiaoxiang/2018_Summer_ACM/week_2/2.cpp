/**
* 2.c
* > author: PhoenixXC
* > email: xuancbm@xiyoulinux.org
* > description: B - 闪现[BFS]

    把初始点放入队列
    从队列中开始找
        如果找到了，打印退出
        如果队列元素没有找到，出列，把与队列元素的有效的关联点放入队列
            记录层数
        继续循环查找

* > created: Tue Jul 31 2018 21:30:16 GMT+0800 (CST)
* > last-modified: Tue Jul 31 2018 21:30:16 GMT+0800 (CST)
*/

#include <stdio.h>
#include <string.h>
#include <queue>
#define MAXN 100005
using namespace std;

queue<int>q;        // 新建队列
int description;    // 目的地
int a, b;
int isVst[MAXN * 2];// 标记是否查询过
int length;         // 查找的bfs层数

void dfs()
{
    int end, flag = 1;
    while (!q.empty())
    {
        if (flag)                       // 标记bfs层数
        {
            end = q.back();
            flag = 0;
        }
        int temp = q.front();           // 记录对首元素
        if (temp == description)
        {
            printf("%d\n", length);     // 找到直接退出
            return;
        }
        q.pop();        
        if (!isVst[temp + 1])
        {
            q.push(temp + 1);
            isVst[temp + 1] = 1;
        }
        if (2 * temp - 2 > a && !isVst[temp - 1])   // 减小搜索范围
        {
            q.push(temp - 1);
            isVst[temp - 1] = 1;
        }
        if (temp < description + 3 && !isVst[temp * 2])
        {
            q.push(temp * 2);
            isVst[temp * 2] = 1;
        }
        if (temp == end)
        {
            length++;
            flag = 1;
        }
    }
    
}

int main(void)
{
    while (scanf("%d %d", &a, &b) == 2)
    {
        while(!q.empty())   // 清空队列
            q.pop();
        memset(isVst, 0, sizeof(isVst));
        length = 0;
        isVst[a] = 1;       // 标记初始点

        description = b;   
        if (a >= b)         // 如果a >= b, 只能倒着一步一步走
            printf("%d\n", a - b);
        else 
            q.push(a);
        dfs();
    }
    return 0;
}
