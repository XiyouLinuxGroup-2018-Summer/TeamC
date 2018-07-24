/**
* 6.1-my_chmod.c     
* @author: PhoenixXC
* @email: xuancbm@xiyoulinux.org
* @description: [Linux_C] -- 改变文件权限(未考虑特殊权限)
* @created: Mon Jul 23 2018 11:29:40 GMT+0800 (CST)
* @last-modified: Mon Jul 23 2018 11:29:40 GMT+0800 (CST)
*/

#include <stdio.h>      // 含有 perror() 的函数原型
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>  // 是Unix/Linux系统的基本系统数据类型的头文件
#include <sys/stat.h>   // 含有 chmod() 函数原型

int main(int argc, char *argv[])
{
    int MODE;           // 权限
    int len;            // 权限参数的长度
    int mode[4] = {0};  // 特殊权限;所有者;用户组;其他用户
    int enum_m[8] = {7, 6, 5, 3, 4, 2, 1, 0};  // 可能的权限设置
    char *path;         // 文件路径

    // 检查参数合法性
    if (argc < 3)
    {
        if (argc == 2)
            printf("my_chmod: \"%s\" 后缺少操作数\n", argv[1]);
        else 
            printf("my_chmod: 缺少操作数\n");
        exit(0);    
    }
 
    // 获取命令行参数 
    len = strlen(argv[1]);       
    if (len > 4 || len < 3)
    {
        printf("my_chmod: 无效模式：%s\n", argv[1]);
        exit(0);
    }
    else 
    {
        int i, j;
        if (len == 3)      // 设置特殊权限为0
        {
            i = 1;
            mode[0] = 0;
            len++;
        }
        else
            i = 0;

        for (i; i < len; i++)
        {
            for (j = 0; j < 8; j++)
            {
                if (argv[1][i] - '0' != enum_m[j])
                    continue;
                else
                    break;
            }
            if (j == 8)
            {
                printf("my_chmod: 无效模式：%s\n", argv[1]);
                exit(0);
            }
            mode[i] = enum_m[j];
        }
    }

    MODE = mode[0] * 8 * 8 * 8 + mode[1] * 8 * 8 + mode[2] * 8 + mode[3];       // 八进制转换, 本应当是宏之间取或运算

    for (int i = 2; i < argc; i++)
    {
        path = argv[i];                 // 获取要转换的文件名
        if (chmod(path, MODE) == -1)
        {
            perror("chmod error!");     // perror() 输出错误信息> %s: ....\n
            // exit(1);                 // 出错则会继续改变下一个文件的权限
        }
    }
    
    return 0;
}