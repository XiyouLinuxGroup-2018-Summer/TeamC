//BFS算法
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int book[1000][1000];
int M, N, S;
int step = 0;
struct node
{
    int x;
    int y;
    int z;
    int l;
};
struct node a[105000];
void bfs()
{

    if (S % 2 != 0)
    {
        printf("NO\n");
        return;
    }
    if (M == N)
    {
        printf("1\n");
        return;
    }
    int head, tail, flag = 0;
    head = 1;
    tail = 1;
    a[tail].x = S;
    a[tail].y = 0;
    a[tail].z = 0;
    a[tail].l = 0;
   book[0][0] = 1;
    int sum = 0, j = 0;
    tail++;
    while (head != tail)
    {
           if ((a[head].x==a[head].y)&&a[head].y==S/2)
            {
              printf("%d\n", a[head].l);
              flag = 1;
              return;
            }
        for (j = 0; j < 6; j++) //不能自己给自己倒水
        {
            if (j == 0) //s->M
            {
                if (a[head].x == 0 || a[head].y == M)
                    continue;
                sum = a[head].x + a[head].y;
                a[tail].l = a[head].l + 1;
                a[tail].x = sum > M ? (sum - M) : 0;
                a[tail].y = sum > M ? M : sum;
                a[tail].z = a[head].z;
                

            }
            if (j == 1) //s->N
            {
                if (a[head].x == 0 || a[head].z == N)
                    continue;
                sum = a[head].x + a[head].z;
                a[tail].l = a[head].l + 1;
                a[tail].x = sum > N ? (sum - N) : 0;
                a[tail].z = sum > N ? N : sum;
                a[tail].y = a[head].y;
                

            }
            if (j == 2) //M->S
            {
                if (a[head].y == 0 || a[head].x == S)
                    continue;
                a[tail].x = a[head].x + a[head].y;
                a[tail].l = a[head].l + 1;
                a[tail].y = 0;
                a[tail].z = a[head].z;
                
            }
            if (j == 3) //headM->N
            {
                if (a[head].y == 0 || a[head].z == N)
                    continue;
                sum = a[head].y + a[head].z;
                a[tail].l = a[head].l + 1;
                a[tail].z = sum < N ? sum : N;
                a[tail].y = sum < N ? 0 : (sum - N);
                a[tail].x = a[head].x;
                  
            }
            if (j == 4) //N->S
            {
              if (a[head].z == 0 || a[head].x == S)
                    continue;
                a[tail].z = 0;
                a[tail].l = a[head].l + 1;
                a[tail].x = a[head].z + a[head].x;
                a[tail].y = a[head].y;

            }
            if (j == 5) //N->M
            {

                if (a[head].z == 0 || a[head].y == M)
                {
                    continue;
                }
                a[tail].z = 0;
                a[tail].l = a[head].l + 1;
                a[tail].y = a[head].y + a[head].z;
                a[tail].x = a[head].x;
            }
             if (!book[a[tail].y][a[tail].z]) 
            {
                book[a[tail].y][a[tail].z] = 1;//tail放在其后
                tail++;
            }
        }
           head++;
    
    }
  if (flag == 0)
    printf("NO\n");
    return;
}
int main()
{
    int t;
    while (scanf("%d %d %d", &S, &M, &N) && S && M && N)
    {
        t = 0;
        if (N > M)
        {
            t = M;
            M = N;
            N = t;
        }
        step = 0;
        memset(book, 0, sizeof(book));
        memset(a, 0, sizeof(a));
        bfs();

    }
    return 0;
}
