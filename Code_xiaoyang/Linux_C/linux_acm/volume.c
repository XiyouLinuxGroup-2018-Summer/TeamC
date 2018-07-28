#include <stdio.h>
#define PI 3.1415927

int main()
{
    float r;
    double v; 
    while(scanf("%f", &r) != EOF)
    {
        v = 4 / 3.0 * PI * r * r * r;
	printf("%.3lf\n",v);
    }
}
