#include<stdio.h>
#include<stdlib.h>
#include<string.h>
int main()
{
    int N,i=0,j=0,k=0;
    int min=0;
    int flag[60];
    memset(flag,0,sizeof(flag));
    scanf("%d",&N);
    char a[100][60];
    for(i=0;i<N;i++)
    {

    getchar();
        scanf("%[^\n]s",a[i]);
       min=strlen(a[i]);
      for(j=0,k=min;(j<=min&&k>=0)&&(j<=k);j++,k--)
    {
        if(a[i][j]!=a[i][k-1])
        {
            flag[i]=1;
        }
    }
        if(flag[i]==1)
            printf("no\n");
        else
            printf("yes\n");
    }
    return 0;
}
