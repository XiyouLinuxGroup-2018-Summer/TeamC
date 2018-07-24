#include<stdio.h>
#include<string.h>
#include<stdlib.h>
int main()
{
    char a[100];
    while(gets(a))//gets函数的返回值问题
    {
      int i=0,j=0;
    j=strlen(a);
    if(a[0]==0)
    {printf("\n");
        continue;} 
    a[0]=a[0]-'a'+'A';
    for(i=0;i<j;i++)
    {
          if(a[i]==' ')
          a[i+1]=a[i+1]-'a'+'A';           
    }
    printf("%s\n",a);
    }
    return 0;
}
