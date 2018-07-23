/**
* 2.c
* @author: PhoenixXC
* @email: xuancbm@xiyoulinux.org
* @description: B - 球体积
* @created: Mon Jul 23 2018 15:54:23 GMT+0800 (CST)
* @last-modified: Mon Jul 23 2018 15:54:23 GMT+0800 (CST)
*/

#include <stdio.h>
#define PI 3.1415927
int main(void)
{
    double r, v;    // float 精度不够
    while (scanf("%lf", &r) == 1)
    {
        v = 4.0 / 3.0 * PI * r * r * r;
        printf("%.3f\n", v);
    }
    return 0;
}