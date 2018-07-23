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


#include<iostream>
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
		case '/': printf("%.2lf", a * 1.0 / b);  cout << endl ; break;
		default : break;
		}
	}
}




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


//  unfinshed !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// #include<iostream>
// #include<set>
// using namespace std;
// int main(void) {
// 	int n , m  , temp ;
// 	set<int> intSet ;
// 	cin >> n >> m ;
// 	if (!n && !m )
// 		return 0 ;
// 	while (n--) {
// 		cin >>  temp  ;
// 		intSet.insert(temp);
// 	}
// 	intSet.insert(m);
// 	auto i = intSet.begin() ;
// 	for( ;i !=  (--intSet.end()) ;i++  )
// 		cout << *i << " ";
// 	cout << *i << endl ;
// }

// #include<iostream>
// #include<cctype>
// using namespace std;
// int main(void) {
// 	string str ;
// 	int n ;
// 	cin >> n ;
// 	while (n--) {
// 		cin >>  str   ;
// 		cout << str << endl ;
// 		for (int i = 0 ;i < str.size() ;++i ) {
// 			if ( str[i] == ' ') {
// 				cout << "no" << endl ;
// 				break ;
// 			}
// 		}
// 		if (str[0] >= '0' && str[0] <= '9') {
// 			cout << "no" << endl;
// 			continue ;
// 		}
// 		//cout << "yes" << endl ;
// 	}
// }


// #include<iostream>
// #include<sstream>
// #include<string>
// using namespace std;
// int main(void) {
// 	string line ,tmp ;
// 	while( getline(cin,line)){
// 		stringstream  ss(line);
// 		while( ss >> tmp ){
// 			tmp[0] = toupper(tmp[0]); 
// 			cout << tmp << " " ;
// 		}
// 		cout <<  endl;
// 	}
// }




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




// #include<iostream>
// #include<sstream>
// #include<string>
// using namespace std;
// int main(void) {
// 	int n ,a,b,c ;
// 	cin >> n ;
// 	while(n--){
// 		cin >> a >> b >> c ;
// 		if((a+b>c) && (b+c > a) && (c+a >b) && (a-b<c) && (b-c <a) && (c-a < b) ){
// 			if( n == 1) cout << "YES";
// 			else 
// 				cout << "YES" << endl ;
// 		}
// 		else {
// 			if( n == 1) cout << "NO"  ;
// 			else 
// 				cout << "NO" <<  endl ;
// 		}
// 	}
// }