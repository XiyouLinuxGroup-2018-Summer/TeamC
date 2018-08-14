// cJson 练习1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"

int main(void)
{
    cJSON * student = cJSON_CreateObject();             // 创建数据根对象
    cJSON_AddNumberToObject(student, "id", 3);          // 添加节点(属性)
    char * out = cJSON_Print(student);                  // 生成适合阅读的json字符串
    char * outfortran = cJSON_PrintUnformatted(student);
    printf("json:\n%s", out);
    printf("\njson for translate:\n%s\n", outfortran);
    free(outfortran);
    free(out);
    cJSON_Delete(student);                              // 释放json对象
}
