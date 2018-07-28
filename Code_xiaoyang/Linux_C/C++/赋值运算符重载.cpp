#include <iostream>
#include <cstring>

class string
{
    private:
        char * str;
    public:
	string():str(new char[1])  { str[0] = 0; }
	const char * c_str { return str; }
	string & operator = (const char * s);
	~string() {delete [] str; }
	string(string & s)
	{
	    str = new char[strlen(s.str) + 1];
	    strcpy(str, s,str);
	}
};

string & string::operator = (const char * s)
{
    delete [] str;
    str = new char[strlen(s) + 1];
    strcpy(str, s);
    return * this;
}

string & string::operator = (const string & s)
{
    if(this == &s)
    {
        return * this;
    }
    delete [] str;
    str = new char[strlen(s.str) + 1];
    strcpy(str, s.str);
    return * this;
}

int main()
{
    string s;
    s = "Good luck";
    cout << s.c_str() << endl;
    s = "shenzhou 8";
    cout << s.c_str() << endl;
    return 0;
}
