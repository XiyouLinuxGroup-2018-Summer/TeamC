//这道题应该是dp问题，可以从后往前推，得到最优化解
#include<stdio.h>
int main()
{
   int tree[200][200],t,c,i,j;
   scanf("%d",&t);//有几组数据
   while(t--)
   {
       scanf("%d",&c);//有几行
       for(i=1;i<=c;i++)
         for(j=1;j<=i;j++)
         {
             scanf("%d",&tree[i][j]);
         }

         for(i=c-1;i>=1;i--)
          for(j=1;j<=i;j++)
          {
              if(tree[i+1][j]<tree[i+1][j+1])
                 tree[i][j]+=tree[i+1][j+1];
                 else
                 tree[i][j]+=tree[i+1][j];
          }
          printf("%d\n",tree[i+1][j-1]);
   }
    return 0;
}