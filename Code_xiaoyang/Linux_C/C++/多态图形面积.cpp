#include <iostream>
#include <cmath>
#include <cstdlib>

#define PI 3.1415

using namespace std;

class cshape
{
    public:
	virtual void printinfo() = 0;
	virtual double area() = 0;
};

class crectangle:public cshape
{
    public:
        int w, h;
	virtual void printinfo();
	virtual double area();
};

class ccircle:public cshape
{
    public:
	int r;
	virtual void printinfo();
	virtual double area();
};

class ctriangle:public cshape
{
    public:
	int a, b, c;
	virtual void printinfo();
	virtual double area();
};

double crectangle::area()
{
    return w * h;
}
void crectangle::printinfo()
{
    cout << "crectangle :" << area() << endl;
}

double ccircle::area()
{
    return PI * r * r;
}
void ccircle::printinfo()
{
    cout << "ccircle :" << area() << endl;
}

double ctriangle::area()
{
    double p = (a + b + c) / 2.0;
    return sqrt(p * (p - a) * (p - b) * (p - c));
}
void ctriangle::printinfo()
{
    cout << "triangle :" << area() << endl;
}


cshape * shape[100];
int compare(const void * s1, const void * s2);

int main()
{
    int n, i;
    crectangle * pr;
    ccircle * pc;
    ctriangle * pt;
    cin >> n;
    for(int i = 0; i < n; i++)
    {
        char c;
	cin >> c;
	switch(c)
	{
	    case 'r':
                pr = new crectangle();
		cin >> pr->w >> pr->h;
		shape[i] = pr;
		break;
	    case 'c':
		pc = new ccircle();
		cin >> pc->r;
		shape[i] = pc;
		break;
	    case 't':
		pt = new ctriangle();
		cin >> pt->a >> pt->b >> pt->c;
		shape[i] = pt;
		break;
	}
    }

    qsort(shape, n, sizeof(cshape *), compare);
    for(i = 0; i < n; i++)
    {
        shape[i]->printinfo();
    }
    return 0;
}

int compare(const void * s1, const void * s2)
{
    double a1, a2;
    cshape ** p1;
    cshape ** p2;
    p1 = (cshape **) s1;
    p2 = (cshape **) s2;

    a1 = (* p1)->area();
    a2 = (* p2)->area();

    if(a1 < a2)
    {
        return -1; 
    }
    else if(a2 < a1)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
