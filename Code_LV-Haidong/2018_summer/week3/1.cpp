include<iostream>
#include<string.h>
#include<stdio.h>
#include<algorithm>
using namespace std;
int a[1005],dp[1005];
const int inf=0x7fffffff;
int main()
{
	int N,i,j,ans,count;
	while(~scanf("%d",&N))
	{
		if(N==0)
			break;
		memset(dp,0,sizeof(dp));		//mement 头文件是 string ,而且必须是C中的头文件，不能是C++ 
		for(i=1;i<=N;i++)
			cin>>a[i];
		for(i=1;i<=N;i++)
		{
			ans=-inf;				//取得最小值，方便进行比较 
			for(j=0;j<i;j++)
			{
				if(a[i]>a[j])				//状态转换方程 
					ans=max(ans,dp[j]);
			}
			dp[i]=ans+a[i];
		}
		sort(dp+1,dp+N+1);			//进行快排，输出最大值 
			cout<<dp[N]<<endl;
	}
	return 0;
}