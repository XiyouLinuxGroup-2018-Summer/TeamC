#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<math.h>
int sum1(int a){//该函数作用是求出数a的所有真因子之和
    int i,sum=0;
    for (i=2;i<sqrt(a);i++)
    {
        if (a%i==0) sum=sum+(a/i)+i;
    }
    return sum+1;
}
int main()
{
    int N;
    scanf("%d",&N);
    int a,b;
    int flag=0;
    while(N--)
    {
        scanf("%d",&a);
        scanf("%d",&b);
       flag=0;
       if((sum1(a)==b)&&(sum1(b)==a))
           flag=1;
       if(flag==1)
           printf("YES\n");
       else
           printf("NO\n");
   }
    return 0;
}
