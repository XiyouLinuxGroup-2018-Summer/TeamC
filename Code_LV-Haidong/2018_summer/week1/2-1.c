#include<stdio.h>
#include<stdlib.h>
#include<string.h>
int main()
{
    int i=0,k=0;
    int j=0;
    char a[100][3];
    char ch;
    memset(a,0,sizeof(a));
     while(scanf("%s",a[i])!=EOF)
     {
         i++;
     }
    for(j=0;j<i;j++)
    {
              if(a[j][k]>a[j][k+1])
              {
                           ch=a[j][k];
                           a[j][k]=a[j][k+1];
                           a[j][k+1]=ch;
              }
              if(a[j][k+1]>a[j][k+2])
              {
                  ch=a[j][k+1];
                  a[j][k+1]=a[j][k+2];
                  a[j][k+2]=ch;
              }
              if(a[j][k]>a[j][k+1])
              {
                    ch=a[j][k];
                    a[j][k]=a[j][k+1];
                    a[j][k+1]=ch;
              }
    }
    for(j=0;j<i;j++)
    {
        for(k=0;k<3;k++)
        {
            printf("%c",a[j][k]);
            if(k<2)
                printf(" ");
        }
        printf("\n");
    }
    return 0;
}
