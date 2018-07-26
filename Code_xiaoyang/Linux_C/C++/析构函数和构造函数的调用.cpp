#include <iostream>

class demo
{
    private:
	    int id;
    public:
	    demo(int i)
	    {
	        id = i;
		cout << "id = " << id << " constructor" << endl;
	    }
	    ~demo()
	    {
	        cout << "id = " << id << " destructor" << endl;
	    }
};

demo d1(1);
void func()
{
    static demo d2(2);
    demo d3(3);
    cout << "func" << endl;
}

int main()
{
    demo d4(4);
    d4 = 6;
    cout << "main" << endl;
    {
        demo d5(5);
    }
    func();
    cout << "main ends" << endl;
    return 0;
}
