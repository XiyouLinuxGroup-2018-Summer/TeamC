/**
* 5.cpp
* > author: PhoenixXC
* > email: xuancbm@xiyoulinux.org
* > description: E - 喝可乐！ 
    初始状态为 S 0 0
    把N和M中大的称为N， 小的称为M，这样最后只需要判断 S' == N' == S / 2 就可以知道是否已经平分
    如果 N == M ,只要一次就可以，继续下一次循环
    将初始状态入队
        如果队列非空
        标记这一层的队尾
            对首元素如果满足状态
                标记搜索成功，然后输出次数，退出
            如果不满足：
                总共6种倒法，判断能否倒且倒完的状态是否被标记过
                    如果可以就入队
        判断已经出队的元素是否是以前被标记的队尾，如果是就更新层数，重设队尾
    如果发现标记搜索失败，printf("NO\n")
* > created: Wed Aug 01 2018 08:17:28 GMT+0800 (CST)
* > last-modified: Wed Aug 01 2018 08:17:28 GMT+0800 (CST)
*/

#include <stdio.h>
#include <string.h>
#include <queue>
#define maxn 101
using namespace std;

typedef struct state        // 记录两个值
{
    int a[3];
}State;
queue<State>q;              // 新建队列
int vst[maxn][maxn];        // 标记状态
int count, flag;            // flag 标记是否被寻找到
int A[3];

void BFS()
{
    int BoE = 1, end[2];
    State tmpState;
    while(!q.empty())
    {
        if (BoE)
        {
            end[0] = q.back().a[0];
            end[1] = q.back().a[1];
            BoE = 0;
        }
        State temp = q.front();
        printf("(%d %d %d)\n", temp.a[0], temp.a[1], temp.a[2]);
        if (temp.a[0] == temp.a[1] && temp.a[1] * 2 == A[0])
        {
            flag = 1;       // 标记已找到
            printf("%d\n", count);
            return;
        }
        q.pop();

        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                if (i == j)
                    continue;
                if (temp.a[i] != 0 && temp.a[j] < A[j])
                {
                    tmpState = temp;

                    if (temp.a[i] <= (A[j] - temp.a[j]))
                    {
                        tmpState.a[i] = 0;     
                        tmpState.a[j] = temp.a[j] + temp.a[i];
                    }
                    if (temp.a[i] > (A[j] - temp.a[j]))
                    {
                        tmpState.a[i] = temp.a[i] - A[j] + temp.a[j]; 
                        tmpState.a[j] = A[j];
                    }
                    
                    if (vst[tmpState.a[0]][tmpState.a[1]] == 0)
                    {
                        vst[tmpState.a[0]][tmpState.a[1]] = 1;
                        q.push(tmpState);
                    }
                }
            }
        }
        if (temp.a[0] == end[0] && temp.a[1] == end[1])
        {
            count++;
            BoE = 1;
        }
    }
}


int main(void)
{
    while (scanf("%d %d %d", &A[0], &A[1], &A[2]) == 3 && A[0] + A[1] + A[2] != 0)
    {   
        flag = count = 0;               // 清除数据
        memset(vst, 0, sizeof(vst));
        while (!q.empty())
            q.pop();

        if (A[1] < A[2])
        {
            int t = A[1];
            A[1] = A[2];
            A[2] = t;
        }

        if (A[1] == A[2])
            printf("1\n");
        else
        {
            State head;
            head.a[0] = A[0];
            head.a[1] = 0;
            head.a[2] = 0;

            vst[A[0]][0] = 1;
            q.push(head);
            BFS();
            if (!flag)
                printf("NO\n");
        }
    }
}
