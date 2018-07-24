#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
int sushu(int n)
{
    int flag=1;
    int k=2;
    for(k=2;k<n;k++)
    {
        if(n%k==0)
        {
            flag=0;
            break;
        }
    }
    return flag;
}
int main()
{
    int x,y;
    int a;
    int i=0;
    int flag=0;
    while(1)
  {  
      flag=0;
     scanf("%d %d",&x,&y);
          if((x==y)&&x==0)
              break;
          for(i=x;i<=y;i++)
          { 
              a=pow(i,2)+i+41;
             flag=sushu(a);
             if(flag==0)
                 break;
        }
             if(flag==1)
             {
                 printf("OK\n");
             }
        else
        {
            printf("Sorry\n");

        }
 }
    return 0;
}
