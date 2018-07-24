#include<stdio.h>
#include<stdlib.h>
#include<string.h>
int main()
{
   int N,M;
   int i=0;
   int j=0,l=0;
   int k=0;
   int m=0;
   int a[40][101];
  while(scanf("%d%d",&N,&M)&&(M!=0||N!=0))
  {  
      l=0;
      k=N;
   for(i=0;i<N;i++)
   {
       scanf("%d",&a[m][i]);
   }
   for(i=0;i<N;i++)
   {
       if(M<a[m][i])
       {
           for(j=N;j>=i;j--)
           {
              a[m][j+1]=a[m][j];
           }
           a[m][i]=M;
           break;
       }
       if(M>a[m][N-1])
       {
           a[m][N]=M;
       }
       if(M==a[m][i])
       {
             for(j=N;j>=i;j--)
             {
                a[m][j+1]=a[m][j];
             }
             a[m][i]=M;
             break;
       }
       if(i==N)
       {
           a[m][i]=M;
       }
  }
      for(i=0;i<=k;i++)
   {   printf("%d",a[m][i]);
       if(i<k)
           printf(" ");
   }
   printf("\n");
   m++;
  }
  return 0;
}
