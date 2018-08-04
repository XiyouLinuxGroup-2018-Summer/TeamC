#include <stdio.h>  
  
char map[101][101];  
int n, m, sum;  
  
void dfs(int i, int j)  
{  
    //若该点不可行或越界，返回  
    if(map[i][j]!='@' || i<0 || j<0 || i>=m || j>=n) return;      
    else    //否则，标记该点为不可行，再往8个方向深搜  
    {  
        map[i][j] = '!';  
        dfs(i-1, j-1);  
        dfs(i-1, j);  
        dfs(i-1, j+1);  
        dfs(i, j-1);  
        dfs(i, j+1);  
        dfs(i+1, j-1);  
        dfs(i+1, j);  
        dfs(i+1, j+1);  
    }  
}  
  
int main()  
{  
    int i, j;  
    while(cin>>m>>n)  
    {  
        if(m==0 || n==0) break;  
        sum = 0;  
        for(i = 0; i < m; i++)  
            for(j = 0; j < n; j++)  
                cin>>map[i][j];  
        for(i = 0; i < m; i++)  
        {  
            for(j = 0; j < n; j++)  
            {  
                if(map[i][j] == '@')  
                {  
                    dfs(i, j);  
                    sum++;  
                }  
            }  
        }  
        cout<<sum<<endl;  
    }  
  
    return 0;  
}  
