/*************************************************************************
	> File Name: 字符排序.cpp
	> Author: Liu Shengxi
	> Mail: 13689209566@163.com
	> Created Time: 2018年07月22日 星期日 19时23分44秒
 ************************************************************************/

/*    A
#include<iostream>
#include<string>
#include<algorithm>
using namespace std;
int main(void) {
	string a ;
	while(cin >> a ){
		sort(a.begin(),a.begin()+3);
		cout << a[0] << ' '<< a[1] << ' '<< a[2] <<  endl ;
	}
}*/


/* 		B
#include<iostream>
#include<string>
#include<algorithm>
using namespace std;
#define PI 3.1415927
int main(void) {
	double r  ;
	while(cin >> r ){
		printf('%.3lf\n',4.0/3*PI*r*r*r);
	}
}*/




/*		C
#include<iostream>
#include<string>
#include<algorithm>
using namespace std;
int main(void) {
	string a ;
	int n ;
	cin >> n ;
	while(n--){
		while(cin >> a ){
			string b =a  ;
			reverse(b.begin(),b.end());
			if(b == a )
				cout << 'yes' <<  endl ;
			else
				cout << 'no'<< endl ;
		}
	}
}*/


/*#include<iostream>
#include<string>
#include<algorithm>
using namespace std;
long long  gcd(long long  a,long long b)
{
    return a%b ?gcd(b,a%b):b;
}
int main(void) {
	int a,b ;
	while(cin >> a >> b  ){
		cout <<  a*b/gcd(a,b) << endl ;
	}

}*/


/*#include<iostream>
#include<string>
#include<algorithm>
#include<stdio.h>
using namespace std;
int main(void) {
	char tag ;
	int  a, b ;
	int n ;
	cin >> n ;
	while (n--) {
		cin >> tag >> a >> b ;
		switch (tag) {
		case '+': cout << a + b << endl; break ;
		case '-': cout << a - b << endl ; break;
		case '*': cout << a*b << endl ; break;
		case '/': if( a%b == 0) printf("%d\n",a/b);
					//当且仅当它不是整数时，结果应该被舍入到2个小数点。
				  else
				  	printf("%.2lf\n", a * 1.0 / b);
				break;
		default : break;
		}
	}
}*/


/*#include<iostream>
#include<string>
#include<algorithm>
#include<stdio.h>
using namespace std;
int main(void) {
	string str ;
	int n  ,count   ;
	cin >> n ;
	while (n--) {
		count = 0 ;
		cin >>  str ;
		for (auto i : str ) {
			  if(isdigit(i))
                count++ ;
		}
		cout << count << endl;
	}
}*/


/*#include<iostream>
#include<algorithm>
#include<vector>
using namespace std;
int main(void) {
	long long n , m  , temp ;
	vector<long long> VV  ;
	while (cin >> n >> m ) {
		if (!n && !m )
			return 0 ;
		while (n--) {
			cin >>  temp  ;
			VV.push_back(temp);
		}
		VV.push_back(m);
		sort(VV.begin(), VV.end());
		size_t i ;
		for (i = 0; i < VV.size() - 1 ; ++i )
			cout << VV[i] << " ";
		cout << VV[i] << "\n" ;
		VV.clear(); //because of you !!!!!!!!!!!!!1
	}
	return 0;
}*/

#include<iostream>
#include<cctype>
#include<string>
using namespace std;
int main(void) {
	string str ;
	long long n ;
	bool tag = true ;
	while (cin >> n ) {
		getchar(); 
		while (n--) {
			getline(cin , str)   ;
			tag = true ;
			/*第一个字符必须是字母（不分大小写）或下划线（_）;*/
			/*那么他的反面是什么呐？？或的反面是且，这是一道数学题，哈哈哈哈哈*/
			if ( !isalpha(str[0]) && str[0] != '_') { 
				cout << "no" << endl ;
				continue ;
			}
			for (int i = 1; i < str.size() ; ++i)
			{
				if ( !isalnum(str[i]) && str[i] != '_') {
					cout << "no" << endl;
					tag = false ;
					break ;
				}
			}
			if (tag)   cout << "yes" << endl ;
		}
	}
}


/*#include<iostream>
#include<string>
using namespace std;
int main(void) {
	string line ,tmp ;
	while( getline(cin,line)){
		line[0] = toupper(line[0]);
		for(int i= 1 ;i < line.size() ;++i ){
			if(line[i-1] == ' ' && islower(line[i]) )
				line[i] = toupper(line[i]);
		}
		cout << line << endl ;
	}
}*/




/*#include<iostream>
#include<sstream>
#include<string>
using namespace std;
int qiuyueshu( int TT){
	int sum = 0;
	for(int i = 1 ;i< TT- 1;i++ ){
		if(TT %i == 0 )
		sum+=i;
	}
	return sum ;
}
int main(void) {
	int m  ;
	cin >> m ;
	int a,b;
	while(m--){
		cin >> a >> b ;
		if((qiuyueshu(a) == b) && (qiuyueshu(b) == a ))
			cout << "YES"<<  endl ;
		else
			cout << "NO"<< endl ;
	}
}*/



/*
#include<iostream>
#include<sstream>
#include<string>
using namespace std;
bool issushu( int TT){
	for(int i = 2 ;i< TT-1 ;i++ ){
		if(TT %i == 0 )
			return false ;
	}
	return true  ;
}
int main(void) {
	int m,n ;
	int tag ;
	while( cin >> m >> n ){
		tag = 0 ;
		if(m == 0 && n== 0 )
			return 0;
		for(int i = m ;i <= n ;i++){
			if( !issushu(i*i+i+41) )
				tag = 1;
		}
		if(tag  == 1 )
			cout << "Sorry"<<  endl;
		else
			cout << "OK"<<  endl;
	}
}*/




/*#include<iostream>
#include<sstream>
#include<string>
using namespace std;
int main(void) {
	int n   ;
	double a,b,c ;
	cin >> n ;
	while(n--){
		cin >> a >> b >> c ;
		if((a+b>c) && (b+c > a) && (c+a >b) && (a-b<c) && (b-c <a) && (c-a < b) ){
				cout << "YES" << endl ;
		}
		else {
				cout << "NO" <<  endl ;
		}
	}
}*/