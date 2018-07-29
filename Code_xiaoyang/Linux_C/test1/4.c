#include <stdio.h>

typedef struct file
{
    int fid;            //文件编号
    char filename[128];
    int mode;           //直接使用整形存储的文件权限
    int size;           //大小
    char data[256];     //文件内容

}myfile;


