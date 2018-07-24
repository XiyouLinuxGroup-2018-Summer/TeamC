#include<stdio.h>
#include<stdlib.h>
#include<string.h>
int main()
{
    int N;
    scanf("%d",&N);
    int i=0,j=0,k=0;
    int count[20];
    memset(count,0,sizeof(count));
    char a[20][100];
    for(i=0;i<N;i++)
    {
        scanf("%s",a[i]);
        k=strlen(a[i]);
        for(j=0;j<k;j++)
        {
             if(a[i][j]>=48&&a[i][j]<=57)
             {
                 count[i]++;
             }
        }
        printf("%d\n",count[i]);
    }
    return 0;
}
