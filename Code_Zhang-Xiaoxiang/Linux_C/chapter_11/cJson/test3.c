#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"

int main(void)
{
    char * out = "{\"name\":\"fengxin\",\"passwd\":\"123\",\"num\":1}";
    cJSON * json, * json_name, * json_passwd, * json_num;

    json = cJSON_Parse(out);
    json_name = cJSON_GetObjectItem(json, "name");          // 获取键值内容
    json_passwd = cJSON_GetObjectItem(json, "passwd");
    json_num = cJSON_GetObjectItem(json, "num");

    printf("解析前：\n");
    char * jsout = cJSON_Print(json);
    printf("%s\n", jsout);
    free(jsout);

    printf("解析后：\n");
    printf("name: %s, passwd: %s, num: %d\n", json_name->valuestring, json_passwd->valuestring, json_num->valueint);
    cJSON_Delete(json);

    return 0;
}
