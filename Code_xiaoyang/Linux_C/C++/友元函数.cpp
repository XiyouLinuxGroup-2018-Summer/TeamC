#include <iostream>

class ccar;
class cdriver
{
    public:
	    void modifycar(ccar * pcar);
};

clacc ccar
{
    private:
	    int price;
	    friend int mostcar(ccar cars[], int total);
	    friend void cdriver::modifycar(ccar * pcar);
};

int mostcar(ccar cars[], int total)
{
    int temp = -1;
    for(int i = 0; i < totla; i++)
    {
        if(cars[i].price > temp)
	{
	    temp = cars[i].price;
	}
    }
    return temp;
}

void cdriver::mostcar(ccar * pcar)
{
    pcar->price += 100;
}

int main()
{
    return 0;
}
