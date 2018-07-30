#include<stdio.h>
#include<stdlib.h>
int count=0;
void dfs(int x ,int y)
{
    count++;
    if((2*x)==y)
    {
      count++;
      printf("%d",count);
       return ;
    }
    dfs(x+1,y);
}
int main()
{
    int a,b;
    while(scanf("%d %d",&a,&b)!=EOF)
    {
        dfs(a,b);
    }
    return 0;
}
