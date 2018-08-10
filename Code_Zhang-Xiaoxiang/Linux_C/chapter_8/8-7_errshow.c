/**
* 8-7_errshow.c
* > author: PhoenixXC
* > email: xuancbm@xiyoulinux.org
* > description: 
* > created: Tue Aug 07 2018 09:16:12 GMT+0800 (CST)
* > last-modified: Tue Aug 07 2018 09:16:12 GMT+0800 (CST)
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

FILE * open_file(char * filename)
{
    FILE * stream;
    errno = 0;

    stream = fopen(filename, "r");
    if (stream == NULL)
    {
        printf("can not open the file %s. reason: %s\n", filename, strerror(errno));
        exit(-1);
    }
    else    
        return stream;
}

int main(void)
{
    char * filename = "test";

    open_file(filename);
    return 0;
}
