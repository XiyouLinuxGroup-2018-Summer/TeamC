#include<stdio.h>
#include<stdlib.h>
#include<string.h>
int main()
{
    int N;
    scanf("%d",&N);
    char a[1001];
    int i=0,j=0;
    int k=0,l=0;
    int flag=1;
    getchar();
    while(1)
    {
        l++;

       gets(a);
        for(i=0;i<N;i++)
    {
       flag=0;
        if((a[0]>='a'&&a[0]<='z')||(a[0]>='A'&&a[0]<='Z')||(a[0]=='_'))
        {  for(j=1;a[j];j++)
         {
             if((a[j]>=48&&a[j]<=57)||(a[j]>=65&&a[j]<=90)||(a[j]>=97&&a[j]<=122)||(a[j]==95))
             {
             }
             else
             {
                 flag=1;
                 break;
         }
         }   
        }
        else
        {
            flag=1;
        }
    }
        if(flag==0)
            printf("yes\n");
        else
              printf("no\n");
        if(l==N)
            break;
    }
    return 0;
}
