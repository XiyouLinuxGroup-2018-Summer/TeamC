#include <iostream>

class myclass
{
    public:
	    ~myclass() { cout << "destructor" << endl; }
};

myclass obj;
myclass fun(myclass sobj)
{
    return sobj;
}

int main()
{
    obj = fun(obj);
    return 0;
}
