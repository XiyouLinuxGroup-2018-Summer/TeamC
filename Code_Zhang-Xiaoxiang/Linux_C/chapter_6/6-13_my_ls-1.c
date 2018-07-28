/**
* 6-13_my_ls.c
* @author: PhoenixXC
* @email: xuancbm@xiyoulinux.org
* @description: my_ls for -a -l -i (-h) -R -r -F -A (-f)
* @created: Wed Jul 25 2018 10:54:31 GMT+0800 (CST)
* @last-modified: Wed Jul 25 2018 10:54:31 GMT+0800 (CST)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <linux/limits.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>

// 命令参数
int Flag_List[] = {0, 1, 2, 4, 8, 16, 32, 64};
char alpha[] = {'0', 'a', 'A', 'l', 'R', 'i', 'r', 'F'};
#define Flag_NONE   Flag_List[0]   // 无参数
#define Flag_a      Flag_List[1]   // -a 显示所有文件
#define Flag_A      Flag_List[2]   // -A 显示除了 . .. 以外的文件
#define Flag_l      Flag_List[3]   // -l 显示具体信息，一行一个文件信息
#define Flag_R      Flag_List[4]   // -R 递归
#define Flag_i      Flag_List[5]   // -i 打印 inode 信息
#define Flag_r      Flag_List[6]   // -r 逆序输出
#define Flag_F      Flag_List[7]   // -F 在文件或目录后打印相关符号

int Flag_cmd = 0;

// 长度限制
#define MAX_C           25  // 限制最多显示的文件列数
#define MAX_USER        16  // 用户名最大长度
#define MAX_GRP         16  // 用户组最大长度
#define MAX_FLAG_NUM    64  // my_ls 参数个数

typedef struct attribute
{
    ino_t   att_inode;                      // i-node 编号
    char    att_mode[11];                   // 类型+权限
    nlink_t att_link;                       // 硬链接数目
    char    att_user[MAX_USER];             // 所有者
    char    att_grp[MAX_GRP];               // 所属组
    off_t   att_size;                       // 文件大小
    char    att_time[32];                   // 最后一次被修改的时间
    char    att_filename[PATH_MAX + 1];     // 文件名
    int     att_flag;                       // 文件类型
    // 每个目录名加 "/" 后缀，每个 FIFO 名加 "|" 后缀， 每个可运行名加 "*" 后缀，sockets　文件加 "="
    // att_flag =  1                     2                      3                        4
}Attr;

typedef struct dirlist
{
    char   name[PATH_MAX + 1];
    struct dirlist * pnext;
}DirList;
/* --------------------------------------------------------- */

// 错误处理
void my_err(const char * err_string, int line)
{
    fprintf(stderr, "line: %d ", line);     // for debug
    perror(err_string);
    // exit(1);     忽略错误
}

// 获取终端宽度
int get_ter_size (void)
{
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return w.ws_col;
}

// Format output 动态数组,　用链表输出时间开销大，数组空间开销大
void PrintB(int n)          // 输出 n 个空格
{
    for (int i = 0; i < n; i++)
        printf(" ");
}
// 多行输出
int Str_UTF(char * str)     // 只占两个宽度的中文字符会识别为三个字节
{
    int num = 0, i;
    while (*str != '\0')    
    {
        if (*str > 0)       // 如果是单字节符号(这里指 英文符号)，正常计算长度
        {
            num++;
            str++;
        }
        else
        {
            // n字节的符号（n > 1），第一个字节的前n位都设为1
            for (i = 7; i >= 0; i--)
                if (!((*str >> i) & 1))  // 右移运算，把第 n+1 位上的０恰好移到 这个字节的最后一位，第7 ~ i+1次,　最后一位会是１
                    break;
            num += 2;           // 中文按两个字节算
            str += 7 - i;       // 跳过这个中文符号(占用7-i个字节)
        }
    }
    return num;
}

int Cal_Print(char b[][NAME_MAX + 10],int a[], int n)       // 暴力运算, 求出打印的列数
{
    int t = get_ter_size();         // 终端宽度
    int i = n < MAX_C ? n : MAX_C;  // 设定最大列数初始值
    int length;                     // 计算字符串长度
   
    for (int j = 0; j < n; j++)
        a[j] = Str_UTF(b[j]);
    for (; i >= 1; i--)
    {
        int c = n % i ? (n / i + 1) : (n / i);
        if ((i - 1) * c >= n)
            continue;

        length = 0;
        int max_len[MAX_C] = {0};

        for (int j = 0; j < i; j++)
        {
            int k = j * c;
            for (int m = 0; m < c && k < n; m++)
            {
                if (a[k] > max_len[j])
                    max_len[j] = a[k];
                k++;
            }
            length += max_len[j];

            if (j != i - 1)
                length += 2;
        }
        if (length < t)
        {
            int kneed = c - i * c + n;

            for (int p = 0; p < c; p++)             // c 行数
            {
                for (int o = 0; o < i - 1; o++)     // i 列数
                {
                    printf("%s", b[p + o*c]);
                    PrintB(max_len[o] - a[p + o*c]);
                    PrintB(2);
                }
                if (kneed)
                {
                    printf("%s", b[p + (i-1) * c]);
                    kneed--;
                }
                printf("\n");
            }
            break;
        }           
        i--;
    }  
    if (!i) // 如果 i == 0, 设 i == 1
    {
        i = 1;
        for (int j = 0; j < n; j++)
            printf("%s\n", b[j]);
    }

    return i;
} 

// 单链表快排
void swap(char * str1, char * str2)
{
    char temp[PATH_MAX + 1];
    strcpy(temp, str1);
    strcpy(str1, str2);
    strcpy(str2, temp);
}
DirList* GetPartion(DirList* pBegin, DirList* pEnd, int CMD)  
{  
    char key[PATH_MAX + 1];
    strcpy(key, pBegin->name);
    DirList* p = pBegin;  
    DirList* q = p->pnext;  
    int flag = CMD ? -1 : 1;    // for -r

    while(q != pEnd)  
    { 
        // 和　strcmp 类似，但对大小写不敏感(for Linux), strcasecmp
        if (flag * strcasecmp(q->name, key) < 0)
        {  
            p = p->pnext;  
            swap(p->name,q->name);  
        }  
  
        q = q->pnext;  
    }  
    swap(p->name,pBegin->name);  
    return p;  
}   
void QuickSort(DirList* pBeign, DirList* pEnd, int CMD)  
{  
    if(pBeign != pEnd)  
    {  
        DirList* partion = GetPartion(pBeign,pEnd, CMD);  
        QuickSort(pBeign,partion, CMD);  
        QuickSort(partion->pnext, pEnd, CMD);  
    }  
}

// 解析文件属性 使用 int lstat(const char *restrict path, struct stat *restrict buf);
void analyze_attribute(struct stat buf, Attr * atb, char * name)
{
    struct passwd * psd;         // 从结构体中获取文件所有者的用户名
    struct group  * grp;         // 从结构体中获取文件所有者所属组的组名

    atb->att_inode = buf.st_ino; // inode 编号
    atb->att_flag = 0;

    // 获取文件类型
    if (S_ISLNK(buf.st_mode))
        atb->att_mode[0] = 'l';
    else if (S_ISREG(buf.st_mode))
        atb->att_mode[0] = '-';
    else if (S_ISDIR(buf.st_mode))
    {
        atb->att_mode[0] = 'd';
        atb->att_flag = 1;
    }
    else if (S_ISCHR(buf.st_mode))
        atb->att_mode[0] = 'c';
    else if (S_ISBLK(buf.st_mode))
        atb->att_mode[0] = 'b';
    else if (S_ISFIFO(buf.st_mode))
    { 
        atb->att_mode[0] = 'f';
        atb->att_flag = 2;
    }
    else if (S_ISSOCK(buf.st_mode))
    {
        atb->att_mode[0] = 's';
        atb->att_flag = 4;
    }
    
    // 考虑特殊权限
    atb->att_mode[1] = (buf.st_mode & S_IRUSR) ? 'r' : '-';
    atb->att_mode[2] = (buf.st_mode & S_IWUSR) ? 'w' : '-';
    atb->att_mode[3] = (buf.st_mode & S_IXUSR) ? 'x' : '-';

    atb->att_mode[4] = (buf.st_mode & S_IRGRP) ? 'r' : '-';
    atb->att_mode[5] = (buf.st_mode & S_IWGRP) ? 'w' : '-';
    atb->att_mode[6] = (buf.st_mode & S_IXGRP) ? 'x' : '-';

    atb->att_mode[7] = (buf.st_mode & S_IROTH) ? 'r' : '-';
    atb->att_mode[8] = (buf.st_mode & S_IWOTH) ? 'w' : '-';
    atb->att_mode[9] = (buf.st_mode & S_IXOTH) ? 'x' : '-';
    atb->att_mode[10] = '\0';

    if (atb->att_mode[0] == '-' && (atb->att_mode[3] == 'x' || atb->att_mode[6] == 'x' || atb->att_mode[9]== 'x'))
        atb->att_flag = 3;

    if (atb->att_mode[0] == '-' && buf.st_mode & S_ISUID)
    {
        if (atb->att_mode[3] = 'x')
            atb->att_mode[3] = 's';
        else
            atb->att_mode[3] = 'S';
    }
    if (buf.st_mode & S_ISGID)
    {
        if (atb->att_mode[6] = 'x')
            atb->att_mode[6] = 's';
        else
            atb->att_mode[6] = 'S';
    }
    if (atb->att_mode[0] == 'd' && buf.st_mode & S_ISVTX)
    {
        if (atb->att_mode[9] = 'x')
            atb->att_mode[9] = 't';
        else
            atb->att_mode[9] = 'T';
    } 

    atb->att_link = buf.st_nlink;                       // 获取链接数

    // 获取用户名和组名
    psd = getpwuid(buf.st_uid);
    grp = getgrgid(buf.st_gid);
    strncpy(atb->att_user, psd->pw_name, MAX_USER - 1);
    strncpy(atb->att_grp,  grp->gr_name, MAX_GRP  - 1);
    atb->att_user[MAX_USER - 1] = '\0';
    atb->att_grp[MAX_GRP - 1]   = '\0';

    atb->att_size = buf.st_size;                        // 获取文件大小

    strncpy(atb->att_time, ctime(&buf.st_mtime), 31);
    atb->att_time[strlen(atb->att_time) - 1] = '\0';    // 去掉换行符

    strncpy(atb->att_filename, name, PATH_MAX);
    atb->att_filename[PATH_MAX] = '\0';                 // 获取文件名
} 

// 单行输出 (考虑链接文件 somelink -> truefile) 有参数 -l 
// 加入 color[printf()]..
void p_single(Attr atb, char * pathname, int isAssign)
{
    // 指定目录 --> 输出路径        [pathname]
    // 否则     --> 只输出文件名    [atb.元素]
    if (Flag_cmd & Flag_i)  // -i
        printf("%8d ", atb.att_inode);
    int flag_for_blank = 0;

    printf("%s ", atb.att_mode);
    printf("%3d  ", atb.att_link);
    printf("%-7s %-7s ", atb.att_user, atb.att_grp);
    printf("%8d ", atb.att_size);
    printf("%20s ", atb.att_time);
    // 文件名有空格的话，用　' ' 包起来
    if (strchr(atb.att_filename, ' ') != NULL)
    {
        printf("'");
        flag_for_blank = 1;
    }
    if (isAssign)
        printf("%s", pathname);
    else
        printf("%s", atb.att_filename);
    if (flag_for_blank == 1)
        printf("'");

    if (atb.att_mode[0] == 'l')     // 如果是链接文件，输出他指向的真实 路径
    {
        char truepath[PATH_MAX+1];
        int len = readlink(pathname, truepath, PATH_MAX);
        truepath[len] = '\0';
        printf(" -> %s", truepath);
    }

    if (Flag_cmd & Flag_F)  // -F
    {
        if (atb.att_flag == 1)
            printf("/");
        else if (atb.att_flag == 2)
            printf("|");
        else if (atb.att_flag == 3)
            printf("*");
        else if (atb.att_flag == 4)
            printf("=");
    }
    printf("\n");
}

// 处理文件
void display_dir(char * path)
{
    // 用链表组织...
    DIR           * dir;
    int       count = 0;
    int   isReverse = 0;
    char  strpath[PATH_MAX + 1];
    struct dirent * ptr;
    struct stat     buf;
    Attr            atb;
    DirList * fidr_list = (DirList*)malloc(sizeof(DirList));      // 链表组织文件 && 目录
    fidr_list->pnext = NULL;
    DirList * pnow = fidr_list, *pnew = NULL;

    dir = opendir(path);
    if (dir == NULL)
    {
        my_err("opendir", __LINE__);
        return ;  
    }
    while ((ptr = readdir(dir)) != NULL)           // 创建链表
    {
        if ((Flag_cmd & Flag_A) && (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0))
            continue;
        if (!(Flag_cmd & Flag_a) && !(Flag_cmd & Flag_A) && ptr->d_name[0] == '.')
            continue;

        pnew = (DirList*)malloc(sizeof(DirList));
        strcpy(pnew->name, ptr->d_name);
        pnew->pnext = NULL;
        pnow->pnext = pnew;
        pnow = pnew;
        count++;
    }
    closedir(dir);

    // 链表排序 -r 为逆序
    if (Flag_cmd & Flag_r)
        isReverse = 1;
    QuickSort(fidr_list->pnext, NULL, isReverse);
    
    // 按格式打印...
    // print for test 
    if (Flag_cmd & Flag_l)      // 如果有 -l 参数, 单行输出
    {
        pnow = fidr_list->pnext;
        while (pnow != NULL)
        {
            // 获取路径
            strcpy(strpath, path);
            strcat(strpath, "/");
            strcat(strpath, pnow->name);
            strpath[strlen(strpath)] = '\0';

            lstat(strpath, &buf);
            analyze_attribute(buf, &atb, pnow->name);
            p_single(atb, strpath, 0);  

            pnow = pnow->pnext;
        }
    }
    else    // 没有 -l，　多行输出 
    {
        char (*b)[NAME_MAX + 10] = (char (*)[NAME_MAX + 10])malloc(sizeof(char) * count * (NAME_MAX + 10));
        int *a = (int*)malloc(sizeof(int) * count);

        pnow = fidr_list->pnext;
        int j = 0;
        while (pnow != NULL)
        {
            strcpy(strpath, path);
            strcat(strpath, "/");
            strcat(strpath, pnow->name);
            strpath[strlen(strpath)] = '\0';

            lstat(strpath, &buf);
            analyze_attribute(buf, &atb, pnow->name);

            if (Flag_cmd & Flag_i)
            {
                char strnode[10];
                sprintf(strnode, "%-8d", atb.att_inode);
                strcpy(b[j], strnode);
                strcat(b[j], "  ");
                strcat(b[j], pnow->name);
            }
            else
                strcpy(b[j], pnow->name);
            if (Flag_cmd & Flag_F)
            {
                if (atb.att_flag == 1)
                    strcat(b[j], "/");
                else if (atb.att_flag == 2)
                    strcat(b[j], "|");
                else if (atb.att_flag == 3)
                    strcat(b[j], "*");
                else if (atb.att_flag == 4)
                    strcat(b[j], "=");
            }
            j++;
            pnow = pnow->pnext;        
        }
        Cal_Print(b, a, count);
        free(b);
        free(a);
    }

    // 从头开始，如果是目录，则 -> 递归
    pnow = fidr_list->pnext;
    while (pnow != NULL && (Flag_cmd & Flag_R))     // 如果有 -R
    {
        strcpy(strpath, path);
        strcat(strpath, "/");
        strcat(strpath, pnow->name);
        strpath[strlen(strpath)] = '\0';
        // 使用lstat, 否则链接文件有时候会陷入死循环 emmmm....
        if (lstat(strpath, &buf) == -1)
        {
            printf("file:%s\n", strpath);
            my_err("stat", __LINE__);
            // exit(1);
        }
        // analyze_attribute(buf, &atb, pnow->name);       
        if (S_ISDIR(buf.st_mode))
        { 
            // 排除. 和　.. 以及链接文件
            if (strcmp(".", pnow->name) != 0 && strcmp("..", pnow->name) != 0)  
            {   
                // 递归
                printf("\n==> %s:\n", strpath);
                display_dir(strpath);
                // 用绝对路径, 直接传文件名文件会找不到, linux 最大路径限制为　PATH_MAX
            }
        }
        pnow = pnow->pnext;
    }

    // 释放空间
    pnow = fidr_list;
    while (pnow != NULL)
    {
        pnew = pnow->pnext;
        free(pnow);
        pnow = pnew;
    }

    return ;
}

int main(int argc, char * argv[])
{
    // ------ -l 总用量 emmm...暂时忽略, 以及忽略iNode长度和文件大小......
    // -------ls 会对目录排序emmmm, 
    // -------目录和文件或许会混起来。。。。
    // -------多个目录会显示目录的路径，再打印相关信息
    // 颜色
    // 设定一个汇总的数组，记录总的大小，最大长度等............
    // -------文件名中如果有空格，用 '<filename>' 包起来
    // 中文排序有点奇怪...
    // -------cmd <参数> <文件\目录, ... >
    // -------cmd <文件\目录, ... > <参数>
    // ------- -R ...
    // -------不能完全按字典序....小写会排在大写的后面emmm..
    // -------ls 输出指定目录信息会打印 path(包含路径名) 如果后面是目录 只会打印 文件名
    // -------命令设置为全局的
    long long   all_size = 0;
    // int         format[15];                  // 格式控制
    int         num_of_flag = 0;                // 带命令参数个数
    int         num_of_file = 0;                // 参数中是文件的数目
    int         num_of_dire = 0;                // 参数中是目录的数目
    int         arr_file[MAX_FLAG_NUM] = {0}; 
    int         arr_dire[MAX_FLAG_NUM] = {0};
    char        path[PATH_MAX + 1];             // 路径
    Attr        atb;
    struct stat buf;

    if (argc > MAX_FLAG_NUM + 1)                // 防止参数太多出问题....
    {
        printf("Warning: too many flags\n");
        printf("Only the first %d parameters are valid\n", MAX_FLAG_NUM);
        argc = MAX_FLAG_NUM;
    }

    // 解析命令 && 区分目录和文件
    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-' && argv[i][1] != '\0')      // 如果是命令，开始解析
        {
            for (int j = 1, k; j < strlen(argv[i]); j++)
            {
                for (k = 1; k < sizeof(Flag_List)/sizeof(int); k++)
                {
                    if (argv[i][j] == alpha[k])
                    {
                        Flag_cmd |= Flag_List[k];
                        break;
                    }
                }
                // 无效命令报错退出
                if (k == sizeof(Flag_List)/sizeof(int))
                {
                    printf("my_ls: 不适用的选项 -- %c\n", argv[i][j]);
                    exit(1);
                }if (k == sizeof(Flag_List)/sizeof(int))
                {
                    printf("my_ls: 不适用的选项 -- %c\n", argv[i][j]);
                    exit(1);
                }
            }
            num_of_flag++;
        }
        else
        {
            strcpy(path, argv[i]);

            // 检查文件/目录是否存在
            if (lstat(path, &buf) == -1)
            {
                my_err("lstat", __LINE__);  // 跳过不正确的文件名/路径
                continue;
            }      

            if (S_ISDIR(buf.st_mode))   // 如果是目录
                arr_dire[num_of_dire++] = i;
            else                        // 如果是文件
                arr_file[num_of_file++] = i;
        }
    }

    // 对参数中的文件和目录排序 emmmm... 冒泡应该够..
    int flag_for_r = 1;
    if (Flag_cmd & Flag_r)
        flag_for_r = -1;
    for (int i = 0, change = 1; i < num_of_file && change; i++)
    {
        change = 0;
        for (int j = 0; j < num_of_file - 1 - i; j++)
        {
            if (flag_for_r * strcmp(argv[arr_file[j]], argv[arr_file[j+1]]) > 0)
            {
                int t = arr_file[j];
                arr_file[j] = arr_file[j+1];
                arr_file[j+1] = t;
                change = 1;
            }
        }
    }
    for (int i = 0, change = 1; i < num_of_dire && change; i++)
    {
        change = 0;
        for (int j = 0; j < num_of_dire - 1 - i; j++)
        {
            if (flag_for_r * strcmp(argv[arr_dire[j]], argv[arr_dire[j+1]]) > 0)
            {
                int t = arr_dire[j];
                arr_dire[j] = arr_dire[j+1];
                arr_dire[j+1] = t;
                change = 1;
            }
        }
    }

    // 如果没有设定目录，设定目录为当前目录 '.'
    if ((num_of_flag + 1) == argc)
    {
        strcpy(path, "./");
        path[2] = '\0';
        display_dir(path);
        return 0;
    }

    // 处理指定的文件
    if (Flag_cmd & Flag_l)                  // 如果有 -l 参数, 单行输出
        for (int i = 0; i < num_of_file; i++)
        {
            strcpy(path, argv[arr_file[i]]);
            lstat(path, &buf);
            analyze_attribute(buf, &atb, path);
            p_single(atb, path, 1);             
        }
    else if (num_of_file)                   // 否则整体输出
    {
        char (*b)[NAME_MAX + 10] = (char (*)[NAME_MAX + 10])malloc(sizeof(char) * num_of_file * (NAME_MAX + 10));
        int *a = (int*)malloc(sizeof(int) * num_of_file);
        for (int j = 0; j < num_of_file; j++)
        {
            strcpy(path, argv[arr_file[j]]);
            lstat(path, &buf);
            analyze_attribute(buf, &atb, path);

            if (Flag_cmd & Flag_i)
            {
                char strnode[10];
                sprintf(strnode, "%-8d", atb.att_inode);
                strcpy(b[j], strnode);
                strcat(b[j], "  ");
                strcat(b[j], path);
            }
            else
                strcpy(b[j], path);
            if (Flag_cmd & Flag_F)
            {
                if (atb.att_flag == 1)
                    strcat(b[j], "/");
                else if (atb.att_flag == 2)
                    strcat(b[j], "|");
                else if (atb.att_flag == 3)
                    strcat(b[j], "*");
                else if (atb.att_flag == 4)
                    strcat(b[j], "=");
            }
        }
        Cal_Print(b, a, num_of_file);

        free(b);
        free(a);
    }

    // 依次处理指定的目录
    for (int i = 0; i < num_of_dire; i++)
    {
        strcpy(path, argv[arr_dire[i]]);
        if (num_of_file != 0 || num_of_dire > 1)
            printf("%s:\n", path);

        display_dir(path);
    }

    return 0;
}
