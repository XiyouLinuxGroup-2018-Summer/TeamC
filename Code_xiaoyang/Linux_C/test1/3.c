#include <stdio.h>
#include <string.h>

struct strange 
{
    char a, b, c, d, e, zero;
};

int main(int argc, char *argv[])
{
    struct strange xp[3];
    char characters[6] = "abcde";

    for (int i = 0; i < 6; i++) 
    {
        *((char *)(xp + 0) + i) = characters[i];   //将xp[0]强制类型转换为char *
	                                           //将characters的字符串逐个拷贝到struct strange xp[0]的每个成员中中
    }

    strcpy((char *)&xp[1], (char *)characters);    //&xp[1]取的是xp[1]对象的地址
                                                   //将characters的字符串逐个拷贝到xp[1]中
    
    memcpy(&xp[2], characters, sizeof(struct strange));  //memcpy函数将字符串characters 逐字节拷贝到xp[2]中
    
    printf("%zu\n", strlen(characters));   //输出characters字符串的长度

    for (int i = 0; i < 3; i++) 
    {
        printf("%c\n", xp[i].e);  //输出struct xp数组每个e成员所保存的字符
    }
}


