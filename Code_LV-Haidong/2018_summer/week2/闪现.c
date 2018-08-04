//求最短时间，BFS算法
//有三种情况分别为x-1,x+1,2x
//如果a小于b这个值的话则会执行两种情况x+1,2x来进行判断
//a大于b这个值的话会执行一种情况a-b
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
typedef struct node
{
   int x;
   int y;
}node;//在这里我使用的队列这个概念
node a[105000];//防止溢出
int book[200000];//用book数组来标记有没有访问这个地方，以防止多次进行访问
int head,tail;//队列的头和尾
void bfs(int c,int b)
{
       if(b==c)///emmmmmm,如果起点与终点是同一位置的话不用移动
       {
         printf("0\n");
         return ;
       }
       if(b<c)//emmm在这里无法实行位移操作
       {
           printf("%d\n",c-b);
           return ;
       }
       head=1;
       tail=1;///初始化队列，保留第一位
       a[tail].x=0;//将time时间初始化
       a[tail++].y=c;//输入终点并且入队，所以tail的值会增加
       book[c]=1;//标记这个点走没走过
       //将每个队列的首进行三次扩展，扩展完毕即出队
       while(head<tail)//队列的头必须小于尾巴
       {
           if(a[head].y>1 && book[a[head].y-1]==0)
           {
               a[tail].x=a[head].x+1;//time加一
              a[tail++].y=a[head].y-1;//在队列最后减去1，并且tail加一入队
             book[a[head].y-1]=1;
           }
           if(a[head].y<100000 && book[a[head].y+1]==0)//为了位一后加1
           {
              a[tail].x=a[head].x+1;
              a[tail++].y=a[head].y+1;
              book[a[head].y+1]=1;
           }
           if(a[head].y<50000 && book[a[head].y*2]==0)
           {
              a[tail].x=a[head].x+1;
              a[tail++].y=a[head].y*2;
              book[2*a[head].y]=1;
           }
           if(a[head].y+1==b || a[head].y-1==b || a[head].y*2==b)
           {
              printf("%d\n",a[head].x+1); 
             
               break;
           }
           head++;//出队,这三种情况每轮一次就要有一个出队
       }

}
int main(void)
{
    int a;
    int b;
    while(scanf("%d %d",&a,&b)!=EOF)
    {
         memset(book,0,sizeof(book));//每次输入的时候需要把book数组清空，不然会影响到计算
         bfs(a,b);
    }
    return 0;
}