#include<stdio.h>
#include<stdlib.h>
#include<string.h>
int main()
{
    int N;
    scanf("%d",&N);
    int i=0;
   long  int a[1001][2];
    double  b[1001];
    char ch[1001];
    int flag=1;
    for(i=0;i<N;i++)
    {
       getchar();
        scanf("%c",&ch[i]);
        scanf("%ld",&a[i][0]);
        scanf("%ld",&a[i][1]);
        if(ch[i]=='+')
        {
            b[i]=a[i][0]+a[i][1];
        }
        if(ch[i]=='-')
        {    b[i]=a[i][0]-a[i][1];
        }
        if(ch[i]=='*')
        {   b[i]=a[i][0]*a[i][1];
        }
        if(ch[i]=='/')
        {
           if(a[i][0]%a[i][1]!=0)
           {
               b[i]=(a[i][0]*1.0)/a[i][1];
               flag=0;
           }
           else
           {   b[i]=a[i][0]/a[i][1];
           }    
        } 
        if(flag==0&ch[i]=='/')
        {
            printf("%.2lf\n",b[i]);
            flag=1;
        }
        else
        printf("%.0lf\n",b[i]);
    }
    return 0;
}
