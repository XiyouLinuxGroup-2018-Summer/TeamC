//这个ls命令计划实现-r,-R,-a,-l -al和颜色区分
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <dirent.h>
#include<stdlib.h>
#include<sys/ioctl.h>
int terminalWidth ;
char filenames[3000][PATH_MAX];
char filenames1[3000][100];
int alfag=0,lflag=0;//参数种类,对-l和-a,-r,-R进行判断
int cnt=0;
void Quicksort(int cnt1,char filenames2[][PATH_MAX],int start);
void recursion(char * path,int alfag);
int ls_prepare(char *path,int lflag);
int ls_prepare(char *pathname,int lflag);
int _ls(char *path,int lflag,int alfag);
int colormode,foreground,background;    
void getcolor( char *filename ){
    struct stat info;    //通过设置可以获取文件属性
    foreground=0;
   lstat( filename,&info );//将文件输入获得赋值给info
//    foreground = 37;
    switch ( (info.st_mode & S_IFMT) ){
        case S_IFREG:               /*regular 普通文件 , 色*/
            {
                foreground = 37;
                    if((info.st_mode&S_IXOTH)||(info.st_mode&S_IXGRP)||(info.st_mode&S_IXUSR))
      {
            foreground = 32;
        }
            break;
            }
        case S_IFLNK:               /*symbolic link 链接文件 , 青蓝色*/
            foreground = 36;
            break;
        case S_IFSOCK:              /*紫红色*/
            foreground = 35;
            break;
        case S_IFDIR:               /*directory 目录文件 , 蓝色*/
        foreground = 34;
            break;
        case S_IFBLK:               /*block special 块设备文件 , 黄色*/
            foreground = 33;
            break;
        case S_IFCHR:               /*character special 字符设备文件 , 黄色*/
            foreground = 33;
            break;
    }
}
struct filename
{
    char filename[PATH_MAX];
    struct filename * next;
};
void Quicksort1(struct filename * pHead,struct filename * tail)
{
    if(pHead->next==tail)
        return ;
    if(pHead->next->next==tail)
    {
            return ;
    }
    struct filename * mid = pHead->next;
    struct filename * p = pHead;
    struct filename * q =mid;
    struct filename * t = mid->next;
    char name[MAXNAMLEN];
    strcpy(name,mid->filename);
    while(t!=tail)
    {
        if(strcmp(t->filename,name)<0)
            p=p->next=t;
        else
            q=q->next=t;
        t=t->next;
    }
    p->next= mid;
    q->next =tail;
    Quicksort1( pHead ,mid );
    Quicksort1( mid  , tail);
}
void list_R(const char * dirname,int lflag)
{
    	DIR		*dir_ptr;		
    	struct dirent	*direntp;		
    	char		*fullpath;
        struct stat info;
//        int i;
        struct filename * pHead,*p1;
        pHead=p1=(struct filename *)malloc(sizeof(struct filename));
        pHead->next=p1->next=NULL;
        printf("%s:\n",dirname);
    	if ( ( dir_ptr = opendir( dirname ) ) == NULL ){
    		fprintf(stderr,"list_R: cannot open %s\n", dirname);
    		return;
    	}
     //  printf("%s:\n",dirname);
//        i=0;
    	fullpath = (char *)malloc(strlen(dirname) + 1 + MAXNAMLEN + 1);
        while ( ( direntp = readdir( dir_ptr ) ) != NULL )
        {   
            struct filename *new1=(struct filename *)malloc(sizeof(struct filename));
            if(alfag==0)
           { 
            if(direntp->d_name[0]=='.')
            {
                continue;
            }
          }          
            
             sprintf(fullpath,"%s/%s",dirname,direntp->d_name);
         if(lflag==3)
             _ls(fullpath,lflag,alfag);
             else
             {
                   strcpy(new1->filename,direntp->d_name);
                   p1->next=new1;
                   p1=new1;
                   new1->next=NULL;
        	}
        }
        Quicksort1(pHead,NULL);
       struct filename *  pTemp=pHead->next;
        struct filename * q=pHead;
        if(pHead->next!=NULL)
        {
            while(pTemp!=NULL)
        {
            getcolor(pTemp->filename);
            printf("\033[%dm%s\033[0m\n",foreground,pTemp->filename);
            free(q); 
            q=pTemp;
            pTemp=pTemp->next;
        }
        }
    	if ( lflag ){
    		rewinddir(dir_ptr);
    		while ( ( direntp = readdir( dir_ptr ) ) != NULL )
            {
             if(alfag==0)
          { 
              if(direntp->d_name[0]=='.')
             {
               continue;
             }
           }
    			sprintf(fullpath,"%s/%s",dirname,direntp->d_name);
            //                printf("%s",direntp->d_name);
           //   puts("");
               lstat(fullpath,&info);
        //       _ls(fullpath,lflag,alfag);
        if(alfag==0)
        {
            if(S_ISDIR(info.st_mode))
                  {
                        putchar('\n');
                        list_R( fullpath, lflag );
                    }
        }
    	else
        {
            if ( S_ISDIR(info.st_mode)&& direntp->d_name[0]!='.')
                    //
                     {
    				putchar('\n');
    				list_R( fullpath, lflag );
    			}
             }
        }
        }
    	closedir(dir_ptr);
    	free(fullpath);
    }
void Quicksort(int cnt1,char filenames2[][PATH_MAX],int start)
{
    int i,j;
    char *name;
    char *name1;
    name=(char *)malloc(MAXNAMLEN);
    name1=(char *)malloc(MAXNAMLEN);
    memset(name,0,MAXNAMLEN);
    memset(name,0,MAXNAMLEN);
    if(start >cnt1)
        return ;
    strcpy(name,filenames2[start]);
    i=start;
    j=cnt1;
    while(i!=j)
    {
        while(strcmp(filenames2[j],name)<=0 && i<j)
            j--;
        while(strcmp(filenames2[i],name)>=0 && i<j)
            i++;
        if(i<j)
        {
                strcpy(name1,filenames2[i]);
           strcpy(filenames2[i],filenames2[j]);
            strcpy(filenames2[j],name1);
      }
    }
    strcpy(filenames2[start],filenames2[i]);
    strcpy(filenames2[i],name);
       Quicksort(i-1,filenames2,start);
       Quicksort(cnt1,filenames2,i+1);
       return ;
       free(name);
       free(name1);
}
void getWidth(){
    struct winsize wbuf;
    terminalWidth = 80;
    if( isatty(STDOUT_FILENO) ){
        if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &wbuf) == -1 || wbuf.ws_col == 0){
        }
        else
            terminalWidth = wbuf.ws_col;
    }
    return ;
}
int getlen_str(char * str)
{
    int res=0,i;
    while(*str!='\0')
    {
        if(*str > 0)
        {
            res++;
            str++;
        }
        else
        {
            for(i=7;i>=0;i--)
            {
                if(!((*str>>7)&1))
                    break;
                res+=2;
                str+=7-i;
             //   if((*str>>7)&1)
            //        break;
            }
        }
    }
    return res;
}
void display_single(int cnt,int alfag)
{
    int i,k,n=0,m=0,j=0,n1;
   n=cnt,n1=cnt;
   i=0;
  Quicksort(cnt,filenames,0);
    while(n>=0)
    {
        n--;
        strcpy(filenames1[i],filenames[i]);
        for(m=0;m<((int)strlen(filenames[i]));m++)
        {
           
            if((filenames[i][m]=='.'&&filenames[i][m-1]=='/'))
             {   
                 break ;
             }
             if(filenames[i][m]=='/')
             {
                 k=m;
           }
        }   
              for(j=0;j<=k;j++)
           {
               filenames[i][j]=' ';
            }
                                  if(alfag==0)
                     {
  
                       if(filenames[i][k+1]=='.')
                       {
                                strcpy(filenames[i]," ");
                        }
                     }
        i++;
    }
    Quicksort(cnt,filenames,0);
    int wordColNUM=20;
     cnt=cnt<wordColNUM?cnt:wordColNUM;//列
    while(cnt>0)
    {  int wordLenMax[4000]={0};//窗口的长度
      int wordRowNum=0;//窗口的行
      j=0;
      int sum=0;
      int length[4000];
      for(j=0;j<n1;j++)
      {
          length[j]=getlen_str(filenames[j]);
      }
     wordRowNum=(n1%cnt)?(n1/cnt):(n1/cnt+1);//行
     if((cnt-1)*wordRowNum>=terminalWidth)
         cnt-- ;
     for(j=0;j<cnt;j++)
     {
         int f=j*wordRowNum;
         for(i=0;i<wordRowNum;i++)//求出行数最宽的是
      {
          if(f>n1)
              break;
          if(getlen_str(filenames[f])>wordLenMax[j])
              wordLenMax[j]=(getlen_str(filenames[f]));
          f++;
      }
         if(j!=cnt-1)
         sum+=wordLenMax[j]+2;
     }
     if(sum<terminalWidth)
     {
       //  cnt--;
         int t=wordRowNum - cnt * wordRowNum +n1;
         for(int p=0;p<wordRowNum;p++)
         {
             for(int o=0;o<cnt-1;o++)
             {
               getcolor(filenames1[p+o*wordRowNum]);
                 printf("\033[%dm%s\033[0m",foreground,filenames[p+o*wordRowNum]);
                 for(int y=0;y<wordLenMax[o]-length[p+o*wordRowNum];y++)
                 printf(" ");
                 for( int g=0;g<2;g++)
                    printf(" ");
          //   printf("\n");
            }
             if(t)
             {
                 getcolor(filenames1[p+(cnt-1 )*wordRowNum]);
                 printf("\033[%dm%s\033[0m",foreground,filenames[p+(cnt-1)*wordRowNum]);
                 t--;
             }
             printf("\n");
         }            
         break;
     }
 //     wordLenMax +=2; //空格防止一组由多行输出;
 //     wordRowNum = terminalWidth/wordLenMax;    //面积除宽等于列的长度
     cnt--;
     if(!cnt)
     {
         cnt=1;
         for(int x=0;x<n;x++)
         {
             getcolor(filenames1[x]);
             printf("\033[%dm%s\033[0m\n",foreground,filenames[x]);
         
         }
     }
    }
      return ;
}
int _ls(char *path,int lflag,int alfag)
{
	int i,n,k;
    int  temp;
	struct stat buf;
	char *out=NULL;
//   char out[PATH_MAX];
	struct passwd *pw=NULL;
	struct group *gr=NULL;
	char *t=NULL;
    char *name=NULL;
    char *name1=NULL;
      name = (char *)malloc(strlen(path) + 1 + MAXNAMLEN + 1);
      name1 =(char *)malloc(strlen(path)+1 +MAXNAMLEN +1);
      out=(char *)malloc(PATH_MAX);
        memset(out,0,PATH_MAX);
   //   j=strlen(out);
    strcpy(name,path);
    strcpy(name1,path);
    for(i=0;i<((int)strlen(name));i++)
    {
         if((name[i]=='.'&&name[i-1]=='/')&&alfag==0)
         {   // cnt--;
             return 0;
         }
         if(name[i]=='/')
            k=i;
    }
    for(i=0;i<=k;i++)
    {
         name[i]=' ';     
    }
    if(lflag==2)
        return 0;
   // lstat(path,&buf);
	if(lstat(path,&buf)<0)
	{
	//	fprintf(stderr,"stat error:%s\n",strerror(errno));
		return 0 ;
	}
    //获取字符串的属性：普通文件-、目录d、字符设备c、块设备b、
    //管道文件p、连接文件l、套接字文件s
    //st.mode表示的是文件的权限和文件的用户
        switch(buf.st_mode & S_IFMT)		//S_IFMT表示位遮罩
	{
	case S_IFREG:	
        {
            printf("-");
        }
		break;
	case S_IFDIR:	
        {
            printf("d");
        }
		break;
	case S_IFCHR:	
        {
            printf("c");
        }
		break;
	case S_IFBLK:	
        {
            printf("b");
        }
		break;
	case S_IFIFO:	
        {
            printf("p");
        }
		break;
	case S_IFLNK:	
        {
            printf("l");
        }
		break;
	case S_IFSOCK:	
        {
            printf("s");
        }
		break;
	}
    //打印文件的读写属性：读r、写w、执行x、无权限-
	for(n=8;n>=0;n--)		
	{
		if(buf.st_mode&(1<<n))
		{
			switch(n%3)
			{
			case 2:
				printf("r");
				break;
			case 1:
				printf("w");
				break;
			case 0:
                {
                    printf("x");
                }
					break;
			default:
				break;
			}
		}
		else
		{
			printf("-");
		}
	}
 
	//硬链接数，此链接非彼链接，指（包含）目录的个数，
	//文件为1，目录起始为2，再加上目录里包含的目录个数（不递归，只一层）
	printf(" %4d ",(int)buf.st_nlink);		
	pw = getpwuid(buf.st_uid);		//所属用户名//可以得到用户指定的信息
                 //根据struct passswd * getpwuid来进行判断
    	printf(" %s ",pw->pw_name);		


	gr = getgrgid(buf.st_gid);		//所属组名
	printf(" %6s ",gr->gr_name);
	printf(" %8ld ",buf.st_size);		//字节计总大小
	t = ctime(&buf.st_atime);	//最后一次访问时间
     printf(" %10.12s ",4+t);
     getcolor(path);
               printf("\033[%dm%4s\033[0m",foreground,name);
	//判断是否为链接，是返回真   
	if(S_ISLNK(buf.st_mode))		
	{
             printf(" -> "); 
		if((temp=readlink(name1,out,PATH_MAX)==-1))//读取链接数失败
		{
		//	printf("readlink error");
		}
        strcat(out,"\0");
		printf("%4s",out);
	}
    printf("\n");
    free(name);
    free(name1);
    free(out);
	return 0;
}
// ls的准备工作
int ls_prepare(char *path,int lflag)		
{
    struct stat buf;		//man lstat可以看到此结构
	DIR *dir;		//opendir会返回一个类似于dir的数据流
	struct dirent *ptr;
    int count = 0;
    //man readdir可以看到此结构
    //获取文件/目录属性并赋值给buf，该函数和lstat一样，
    //只是当w为链接时，指代他本身，并不存在文件
lstat(path,&buf);
    /*	if(lstat(path,&buf)<0)		
	{
		fprintf(stderr,"stat error:%s\n",strerror(errno));
		return -1;
	}*/
    dir=opendir(path);
    while((ptr= readdir(dir))!=NULL)
    {
            count++;
    }
    closedir(dir);
    printf("%d",count);
     int i,len=strlen(path);
     dir=opendir(path);
     for(i=0;i<count;i++)
     {
         ptr=readdir(dir);
         if(ptr==NULL)
         {
             perror("readdir");
         }
         strcpy(filenames[i],path);
         filenames[i][len]='\0';
         strcat(filenames[i],ptr->d_name);
         filenames[i][len+strlen(ptr->d_name)]='\0';
     }
     cnt=i;
	//S_IRUSR表示读权限,S_IWUSR表示写权限 
  Quicksort(cnt-1,filenames,0);
    if(lflag==0)
    {
        display_single(cnt,alfag);
        return 0;
    }
        for(i=0;i<count;i++)
		{
            _ls(filenames[i],lflag,alfag);
	}
   closedir(dir);	
	return 0;
}
int main(int argc,char ** argv)
{
   getWidth();
    char param[32];//用来保存命令行的参数,命令行的形式一般为 -l -al -a
   char path[81];
   struct stat buf;
   int i,j,k,num=0;
    //命令行中实现解析
    j=0;
   for(i=1;i<argc;i++)
   {
       if(argv[i][0]=='-')
       {
           for(k=1;k<(int)strlen(argv[i]);k++,j++)
           {
                param[j] = argv[i][k];
           }
           num++;
       }
   }
  for(i=0;i<j;i++)
  {
        if(param[i]=='a')
        {
            alfag=1;
            continue;
        }
        if(param[i]=='l')
        {
          lflag+=1;
          continue;
        }
        if(param[i]=='R')
        {
            lflag=lflag+2;
            continue;
        }
  }
  param[j]='\0';
  //如果在其后没有输入文件名或者目录，默认为当前目录
  if((num+1)==argc)
  {
      strcpy(path,"./");
      path[2]='\0';
      ls_prepare(path,lflag);
      return 0;     
  }
  i=1;
  do
  {
      if(argv[i][0]=='-')
      {
          i++;
          continue;
      }
      else
      {
          strcpy(path,argv[i]);
          if(stat(path,&buf)==-1)
          {
              perror("stat");
          }
          if(S_ISDIR(buf.st_mode))//argv[i]是一个目录
           //   ，如果目录的最后一个字符不是'/',添加
          {
                    if(path[strlen(argv[i])-1]!='/')
                    {
                        path[strlen(argv[i])]='/';
                        path[strlen(argv[i])+1]='\0';//添加文件结束符号
                    }
                    else  
                        path[strlen(argv[i])]='\0';
                    if(lflag>=2)
                    {
                      list_R(path,lflag);
                       return 0;
                     }  
                        ls_prepare(path,lflag);
                        i++;
          }
          else{
                        display_single(cnt,alfag);
                        i++;
          }
      }
  }while(i<argc);
return 0;
}

