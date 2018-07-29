#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct strange {                    // 创建一个结构体类型，包含6个字符型变量
    char a, b, c, d, e, zero;
};

int main(int argc, char *argv[])
{
    struct strange xp[3];           // 新建一个有三个元素的数组，数组的元素是struct strange类型的
    int characters[6] = {'a', 'b', 'c', 'd', 'e', '\0'};

    for (int i = 0; i < 6; i++) {   // 把characters数组中的元素依次赋值给xp[0]的a,b,c,d,e,zero
                                    // xp[0].e = 'e'
        *((char *)(xp + 0) + i) = characters[i];
        // 强制把xp[0]转化为char *，依次对其元素赋值
    }

    strcpy((char *)&xp[1], (char *)characters);
    memcpy(&xp[2], characters, sizeof(struct strange));     
    // 把sizeof(struct strange)的内容复制到xp[2]中， int有4个字节， char 1个字节, 
    // 所以characters内存上是：
    // a     0  0  0        b      0 0 0    c 0 0 0 ....
    // xp[2].a ....       xp[2].e = 'b'
    printf("%zu\n", strlen((char *)characters));    

    for (int i = 0; i < 3; i++) {
        printf("%c\n", xp[i].e);
    }
}