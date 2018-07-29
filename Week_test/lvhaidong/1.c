//想要对一级指针进行修改的话，必须使用二级指针才可以
//想要存储字符串的话得用name来申请一段内存
//使用二级指针的话就得添加星号
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
typedef struct tag_unimportant {
    char *name;
    int num;
} A;
void func(A **a)
{
   *a = (A *)malloc(sizeof(A));
   (*a)->name=(char * )malloc(17);
    strcpy((*a)->name, "Xiyou Linux Group");
   (*a)->num = 20180728;
}
int main()
{
    A *a;
    func(&a);
    printf("%s %d\n", a->name, a->num);
    free(a);
    return 0;
}
