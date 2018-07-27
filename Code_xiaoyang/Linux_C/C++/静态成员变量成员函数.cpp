#include <iostream>

class crectangle
{
    private:
	    int w, h;
	    static int number;
	    static int area;
    public:
	    crectangle(crectangle & r);
	    crectangle(int w_, int h_);
	    ~crectangle();
	    static void printtotal();
};

crectangle::crectangle(crectangle & r)
{
    w = r.w;
    h = r.h;
    number++;
    area = w * h;
}

crectangle::crectangle(int w_, int h_)
{
    w = w_;
    h = h_;
    number++;
    area += w * h;
}

crectangle::~crectangle()
{
    number--;
    area -= w * h;
}

void crectangle::printtotal()
{
    cout << number << "," << area << endl;
}

int crectangle::number = 0;
int crectangle::area = 0;

int main()
{
    crectangle r1(3, 3), r2(2, 2);
    rectangle::printtotal();
    r1.printtotal();
    return 0;
}
