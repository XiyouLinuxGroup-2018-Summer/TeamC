#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/limits.h>
#include <sys/stat.h>
#include <sys/types.h>

void my_err(char * err_string, int line)
{
    printf("%s: %d\n", err_string, line);
    perror(err_string);
    // exit(1);
}

typedef struct file
{
    int fid;                // 文件编号
    char filename[128];
    int mode;               // 直接使用整型存储的文件权限(例如 755 存储为 493 也就是 0755)                       
    int size;               // 大小
    char data[256];         // 文件内容
}myfile;

typedef struct stFile
{
    myfile file;
    struct stFile * next;
}FileList;

int main(void)
{
    int count = 0;  // for fid
    int fd;
    DIR * dir;
    struct dirent * pt;
    struct stat  buf;
    char pathname[PATH_MAX] = "/lib/";
    char PATH[PATH_MAX];
    char tempdata[512];

    FileList * list = (FileList*)malloc(sizeof(FileList));
    list->next = NULL;
    FileList * pnow = list, *pnew;

    if ((dir = opendir(pathname)) == NULL)
        my_err("opendir", __LINE__);

    while ((pt = readdir(dir)) != NULL)
    {
        if (strcmp(pt->d_name, ".") == 0 || strcmp(pt->d_name, "..") == 0)
            continue;
        if (S_ISDIR(buf.st_mode))  
            continue;

        strcpy(PATH, pathname);
        strcat(PATH, pt->d_name);

        if (lstat(PATH, &buf) == -1)
            my_err("stat", __LINE__);
        
        pnew = (FileList*)malloc(sizeof(FileList));
        pnew->next = NULL;

        pnew->file.fid = ++count;
        strcpy(pnew->file.filename, pt->d_name);
        pnew->file.mode = buf.st_mode;
        pnew->file.size = buf.st_size;

        if ((fd = open(PATH, O_RDONLY)) == -1)
            my_err("open", __LINE__);
        if (read(fd, pnew->file.data, 256) == -1)
        {
            printf("file name %s:\n", PATH);
            printf("fid : %d\n", pnew->file.fid);
            my_err("read", __LINE__);
        }    
        close(fd);

        pnow->next = pnew;
        pnow = pnew;
    }
    closedir(dir);

    printf("count = %d\n", count);
    pnow = list->next;
    while(pnow)
    {
        if ((fd = open("./allfiles.dat", O_RDWR|O_CREAT|O_TRUNC, S_IRWXO)) == -1)
            my_err("open", __LINE__);
        if (write(fd, pnow->file.data, 256) < 0)
            my_err("write", __LINE__);
        pnow = pnow->next;
    }

    return 0;
}