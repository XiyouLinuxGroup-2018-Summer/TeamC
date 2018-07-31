//这个题依然使用深度搜索来进行
//因为每个数字都必须为1或者0
//所以有两种遍历的方式
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
int f;
void dfs(  unsigned long long int  x,int k,int n)
{
        if(f)
            return ;
         if(x%n==0)
         {
             printf("%llu\n",x);
             f=1;
             return ;
         }
         if(k==19)//在这里应该考虑x的范围，如果超范围的话会溢出输出错误数据
             return ;
         dfs(x*10,k+1,n);//因为它必须为1或者0
         dfs(x*10+1,k+1,n);
}
int main()
{
    int n;
    while(scanf("%d",&n)&&n)
    {
        f=0;
        dfs(1,0,n);
    }
}
