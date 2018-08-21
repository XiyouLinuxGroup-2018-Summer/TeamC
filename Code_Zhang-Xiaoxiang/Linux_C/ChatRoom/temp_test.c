// file: test ChatRoom
#include <stdio.h>
#include<sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>


int main(void)
{
    int i;
    loop:for (i = 0; i < 3; i++)
    {
        printf("hello world.\n");
        sleep(1);
    }
    printf("========================\n");

    if (i == 3)
        goto loop;
}
