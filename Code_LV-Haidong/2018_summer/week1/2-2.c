#include<stdio.h>
#define PI 3.1415927
int main()
{
    double n ;
    while( scanf("%lf",&n) != EOF ) {
        printf("%.3lf\n",4.0/3*PI*n*n*n);
    }
    
    return 0;
}
