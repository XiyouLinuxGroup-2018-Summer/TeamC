#include <iostream>

class comp
{
    public:
	    double real, imag;
	    void print() { cout << real << "," << imag << endl; }
	    comp(double i, double j):real(i), imag(j) {  }
	    comp addone()
	    {
	        this->real++;
		this->print();
		return * this;
	    }
};

int main()
{
    comp c1(1, 1), c2(0, 0);
    c2 = c. addone();
    return 0;
}
