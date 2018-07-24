/**
* 12.c
* @author: PhoenixXC
* @email: xuancbm@xiyoulinux.org
* @description: L - 首字母大写 
* @created: Tue Jul 24 2018 09:27:02 GMT+0800 (CST)
* @last-modified: Tue Jul 24 2018 09:27:02 GMT+0800 (CST)
*/

#include <stdio.h>
#include <string.h>
int main(void)
{
    char str[105];
    str[0] = ' ';
    while (gets(str+1) != NULL)   // 读取失败或遇到文件尾会返回 NULL
    {
        int len = strlen(str);
        for (int i = 0; i < len;)
        {
            int flag = 0;
            while (str[i] == ' ' && i < len) // 跳过空格
            {
                i++;
                flag = 1;
                continue;
            }
            if (flag == 1)
            {
                if (str[i] >= 'a' && str[i] <= 'z')
                    str[i] -= 32;
            }
            i++;
        }
        puts(str+1);
    }
    return 0;
}