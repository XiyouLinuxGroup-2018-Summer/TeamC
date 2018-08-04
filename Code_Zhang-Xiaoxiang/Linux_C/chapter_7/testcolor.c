#include <stdio.h>

#define CLOSE "\033[0m"
#define BLOD  "\033[1m"
#define BEGIN(x,y) "\033["#x";"#y"m"

int main(void)
{
    printf("test color for myshell\n");
    printf(BEGIN(48,32)"Hello world\n"BLOD);
    printf(CLOSE);
    // printf("=======================\n");
    // printf(BEGIN(45,37)"Hello world\n");
    printf("\033[?25h\n");

}
