#include <stdio.h>      // 头文件包含
#include <stdlib.h>
#include <string.h>

typedef struct tag_unimportant {
    char *name;
    int num;
} A;

void func(A **a)
{
    *a = (A *)malloc(sizeof(A));    // 为*a申请一段内存
    (*a)->name = (char*)malloc(sizeof(char) * 20);  // 为*a->name申请一段内存
    strcpy((*a)->name, "Xiyou Linux Group");
    (*a)->num = 20180728;
}

int main(int argc, char *argv[])
{
    A *a;
    func(&a);       // 指针a没有赋值，传递指针a的地址
    printf("%s %d\n", a->name, a->num);
    return 0;
}