#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"

int main()
{
    cJSON * usr;
    cJSON * arry;

    usr = cJSON_CreateObject();                       // 创建根数据对象
    cJSON_AddStringToObject(usr, "name", "xxx");  // 加入键值，加字符串
    cJSON_AddStringToObject(usr, "passwd", "xxx"); 
    cJSON_AddStringToObject(usr, "lastest time", "2018-8-14/14:02:98");
    cJSON_AddNumberToObject(usr, "num", 1);           // 加整数

    char * out = cJSON_Print(usr);                    // 将json形式打印成正常字符串形式
    char * unout = cJSON_PrintUnformatted(usr);        // 将json形式打印为无格式的形式 
    printf("json fromat\n%s\n",out);
    printf("json unformat\n%s\n", unout);

    // 释放内存  
    cJSON_Delete(usr);  
    free(out);     
}
