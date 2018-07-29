#include<sys/types.h>
#include<dirent.h>
#include<unistd.h>
#include<stdio.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include<stdlib.h>
typedef struct file{
    int fid;
    char filename[128];
    int mode;
    int size;
    char data[256];
    struct file *next;
}myfile;
long  long int i=0;
void list(const char *dirname )
{
    DIR * dir_ptr;
    struct dirent * direntp;
    char *fullpath;
    struct stat info;
    struct file * pHead, *p1;
    FILE *fp,*fp1;
    int  temp;
	struct stat buf;
	char *out=NULL;
//   char out[PATH_MAX];
      out=(char *)malloc(PATH_MAX);
        memset(out,0,PATH_MAX);
    pHead=p1=(myfile *)malloc(sizeof(myfile));
    pHead->next=p1->next=NULL;
    p1->fid=1;
    if((dir_ptr = opendir(dirname))==NULL)
    {
       fprintf(stderr,"cannot open %s\n",dirname);
       return ;
    }
    fp=fopen("./allfile.dat","at+");
    if(fp==NULL)
        printf("errod");
    fullpath = (char *)malloc(strlen(dirname)+1+MAXNAMLEN+1);
    while((direntp = readdir( dir_ptr  ))!=NULL)
     {
           myfile *new1 =(myfile *)malloc(sizeof(myfile));
          if(direntp->d_name[0]=='.')
          {
              continue;
          }
          lstat(direntp->d_name,&buf);
          new1->size=buf.st_size;
          new1->mode=buf.st_mode;
          if((buf.st_mode &S_IFMT)==S_IFREG)
          {         fp1=fopen("direntp->d_name","r+");
          fread(new1->data,1,255,fp1);
          fclose(fp1);
          }
           sprintf(fullpath,"%s%s",dirname,direntp->d_name);
              strcpy(new1->filename,direntp->d_name);
              i++;
              new1->fid=i;
              p1->next=new1;
                  p1=new1;
              new1->next=NULL;
          }
      myfile * pTemp =pHead ->next;
      if(pHead->next!=NULL)
      {
                while(pTemp!=NULL)
                {
                    printf("%s\n",pTemp->filename);
                             if(S_ISLNK(buf.st_mode))
        {
            if((temp=readlink(pTemp->filename,out,PATH_MAX)==-1))//读取链接数失败
            {
            }
            strcat(out,"\0");
            fprintf(fp,"%s",out);
            continue;
        }            
                           if(pTemp->fid ==20 && pTemp->fid ==18)
                        {
                         pTemp->mode=777;
                        }
                    if(pTemp->fid!=7&&pTemp->fid!=28)
                    {
                        fprintf(fp,"%d %s %d %d %s\n",pTemp->fid,pTemp->filename,pTemp->mode,pTemp->size,pTemp->data);
                    }
                    pTemp=pTemp->next;
                }
      }
      rewinddir(dir_ptr);
      while((direntp = readdir(dir_ptr))!=NULL)
      {
          if(direntp->d_name[0]=='.')
              continue;
          sprintf(fullpath,"%s/%s",dirname,direntp->d_name);
          lstat(fullpath,&info);
         if(S_ISDIR(info.st_mode)&&(direntp->d_name[0]!='.'))
         {
             putchar('\n');
             list(fullpath);
         }

      }
      closedir(dir_ptr);
      fclose(fp);
     pTemp=pHead->next;
      myfile * q=pHead;
           if(pHead->next!=NULL)
        {
            while(pTemp!=NULL)
        {
            free(q); 
            q=pTemp;
            pTemp=pTemp->next;
        }
        }
      free(fullpath);
}    
int main()
{
    list("/lib/");
    return 0;
}
