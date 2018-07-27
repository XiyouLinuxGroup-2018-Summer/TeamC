#include <iostream>
#include <cstdlib>
using namespace std;

class date
{
	private:
		int year;
		int month;
		int day;
		bool isleap();
	public:
		date(int a,int b,int c);
		void adddate();
		void setdate(int a,int b,int c);
		void printdate();
};

date::date(int a,int b,int c)
{
	setdate(a,b,c);
}

void date::setdate(int a,int b,int c)
{
	year = a;
        month = b;
	day = c;
}

void date::printdate()
{
	cout << year << "/" << month << "/" << day << endl ;
}

bool date::isleap()
{
	if(year % 400 == 0)
	        return true;
	else if(year % 4 == 0 && year % 100 != 0)
		return true;
	else 
		return false;
}

void date::adddate()
{
	bool is;
	is = isleap();
	if(month == 1||month == 3||month == 5||month == 7||month == 8||month ==10)
	{
		if(day == 31)
		{
			month++;
			day = 1;
		}
		else
			day++;
	}
	else if(month == 4||month == 6||month == 9||month == 11)
	{
		if(day == 30)
		{
			month++;
			day = 1;
		}
		else
			day++;
	}
        else if(month == 12)
	{
		if(day == 31)
		{
			year++;
			month = 1;
			day = 1;
		}
		else
			day++;
	}
	else(month == 2)
	{
		if((is && day == 29) || (!is && day == 28))
		{
			month++;
			day = 1;
		}
		else
			day++;
	}
}

int main()
{
	date d(2013,10,31);
	int a,b,c;
	cout << "current date(year/month/day) is ";
        d.printdate();
	d.adddate();
	cout << "add one day to current date is";
	d.printdate();
	cout << "please input date(year/month/day) ";
	cin >> a >> b >> c;
	d.setdate(a,b,c);
	a.adddate();
	cout << "after set date(year/month/day) is";
	d.printdate();
        return 0;
}
