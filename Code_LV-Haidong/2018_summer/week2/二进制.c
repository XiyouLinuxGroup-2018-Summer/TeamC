#include<stdio.h>
#include<stdlib.h>
long long int mode[60001];
int main()
{
    int n;
    int i;
    while(scanf("%d",&n)&&n)
    {
        for( i=1;;i++)
        {
          mode[i]=mode[i/2]*10+i%2;
          if(mode[i]%n==0)
          {  
              printf("%lld\n",mode[i]);
              break;
          }
       }
    }
    return 0;
}
