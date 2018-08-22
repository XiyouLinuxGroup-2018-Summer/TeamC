#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"

int create_js(void)
{
    cJSON * root, * js_body;
    root = cJSON_CreateArray();
    cJSON_AddItemToArray(root, cJSON_CreateString("hello world"));
    cJSON_AddItemToArray(root, cJSON_CreateNumber(10));
    char * s = cJSON_Print(root);
    if (s)
    {
        printf("%s\n", s);
        free(s);
    }
    if (root)
        cJSON_Delete(root);

    return 0;
}

int main(int argc, char **argv)
{
    create_js();
    return 0;
}
