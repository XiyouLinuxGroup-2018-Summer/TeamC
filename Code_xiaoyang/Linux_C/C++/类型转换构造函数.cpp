#include <iostream>

class comp
{
    private:
	    double real, imag;
    public:
	    comp(int i)
	    {
	        cout << "intconstructor called" << endl;
		real = i;
		imag = 0;
	    }
	    comp(double i, double j):real(i), imag(j)    {   }
};

int main()
{
    comp c1(7, 8);
    comp c2 = 12;
    c1 = 9;
    cout << c1.real << "," << c1.imag << endl;
    return 0;
}
