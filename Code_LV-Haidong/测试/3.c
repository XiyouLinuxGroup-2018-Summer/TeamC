//此程序过不了编译
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
struct strange {
    char a, b, c, d, e, zero;
};
int main()
{
    struct strange xp[3];//申请了结构体数组，每个结构体数组中函数六个变量
    //这种存储方式是错误的，应该使用char 类型来存储，使用双引号本来就是字符串的形式
    int characters1[6] = {'a','b','c','d','e'};//以int的方式存储abcde,本来abcde各占一个字节，结果要以4个字节存储
    for (int i = 0; i < 6; i++) {
        *((char *)(xp + 0) + i) = characters1[i];
    }//使用强制类型转化把 characters中的数据转化为char 类型
    //这种方式是二维数组的第一排转化
    strcpy((char *)&xp[1], (char *)characters1);//把characters强制类型转化为（char *)
    //然后付给它
    memcpy(&xp[2], characters1, sizeof(struct strange));
    //struct strange 只有6个字节,而characters有24个字节，会造成数据丢失
    //根据struct strange 的大小,将characters的值付给它
    printf("%zu\n", strlen((char *)characters1));
    //将int * 强制类型转化为char * 类型的指针，在64位的平台上任何一个指针的类型都将会是8个字节
    //而在32位的情况下是4个字节
    for (int i = 0; i < 3; i++) {
        printf("%c\n", xp[i].e);
    }//输出xp[i].e
}
