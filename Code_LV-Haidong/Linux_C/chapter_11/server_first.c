//C/S模型实现多用户通信
//C-S-C模型实现多个用户通信,群聊的话，通过一个C->S S向其他C进行广播
//通过客户端转发消息
#include "my_chattingroom.h"
#include <sys/epoll.h>
#include <fcntl.h>
#include "cJSON.h"
#include <errno.h>
#include <pthread.h>
#include <mysql/mysql.h>
#define SERV_PORT 8080 //务器端的端口
#define LISTENQ 20     //接请求队列的最大长度


struct chatting  * head;
MYSQL *conn; //验证登陆等功能需要
void regist(qq information, int conn_fd);//注册
void login(qq information, int conn_fd);//登陆i
void chat_input(qq information, int conn_fd);//使用epoll来接受套接字的变化
void chat_friend(qq information,int conn_fd);//与好友进行聊天
void chat_list(qq information,int conn_fd);//展示好友列表
void chat_delete(qq information,int conn_fd);//删除好友
void chat_black(qq information,int conn_fd);//黑名单
int  search(struct chatting * head,char * to);
void create_group(qq information,int conn_fd);
void group_delete(qq information,int conn_fd);
void group_member_list(qq information,int conn_fd);
void group_set_guanli(qq  information,int conn_fd);
void group_inivt_mebmer(qq information, int conn_fd);
void group_kick_out_members(qq information,int conn_fd);
void chatting_group(qq information,int conn_fd);//一对多的聊天
void send_file(qq information,int conn_fd);//给别的客户端发送文件
void init_file(qq information,int conn_fd);//判断此好友是不是存在
void add_offline_news(qq information,char * go,char * come);//go和come 表示我想要给谁发消息
void look_offline_news(qq information,int conn_fd);//查找离线消息
void chatting_news(qq information,int conn_fd);//获取好友之间的消息记录
void group_news(qq information,int conn_fd);//获取群的聊天记录
void group_list(qq information,int conn_fd);

void chatting_news(qq information,int conn_fd)
{
    int t ,y;
    char query1[100];

    MYSQL_RES *result ;
    MYSQL_ROW row ;
    qq server_information;
    result = mysql_store_result(conn);//获取结果集
    mysql_free_result(result);
    //执行一个mysql的select 语句，应当释放一下结果集,不然会有错误
    sprintf(query1, "select  * from  chat_record where my_number=%s or friend_number=%s ;",information.number,information.to) ;

    t= mysql_real_query(conn,query1,strlen(query1));
    if(t != 0 )    
    {
        printf("%s\n",mysql_error(conn));
        my_err("select ",__LINE__);
    }  
    result = mysql_store_result(conn) ;
    if(result == NULL)
    {
        strcpy(server_information.record,"暂无消息记录");
        server_information.flag = 19;
        send(conn_fd,&server_information,sizeof(qq),0);
        return ;
    }
    while(row = mysql_fetch_row(result))
    {
            strcpy(server_information.number,row[0]);
            strcpy(server_information.to,row[1]); 
            strcpy(server_information.record,row[2]);
            server_information.flag = 19;
            send(conn_fd,&server_information,sizeof(qq),0);
            //发送给客户端
    }
    mysql_free_result(result);
}

void group_news(qq information,int conn_fd)
{
    MYSQL_RES * result;
    MYSQL_ROW  row;
    char query1[200];
    int t,y;
    memset(query1,0,200);
    sprintf(query1,"select content from group_%s where group_name = '%s' ; ",information.number,information.to);
    qq group_server;
    
    strcpy(group_server.to,information.to);
    t = mysql_real_query(conn,query1,sizeof(query1));
    if(t != 0)
    {
      my_err("selct ",__LINE__);
      printf("%s\n",mysql_error(conn));
    }
    result = mysql_store_result(conn);
   while(row = mysql_fetch_row(result))
    {
        //返回结果集中的列数
        for(y = 0;y < mysql_num_fields(result);y++)   
        {
            strcpy(group_server.record,row[y]);      
            group_server.flag = 20;
            send(conn_fd,&group_server,sizeof(qq),0);
            //发送给客户端
        }
    }

}

void add_offline_news(qq information,char * go,char * come)
{
   MYSQL_RES * result;
   result = mysql_store_result(conn);
   mysql_free_result(result);
    
   char query1[200];
   sprintf(query1,"insert into off_line_record_%s (go,come,record) values('%s','%s','%s');",information.number,go,come,information.record);
   int t = mysql_real_query(conn,query1,strlen(query1));
   if( t != 0 )
   {
       my_err("insert into ",__LINE__);
       printf("%s\n",mysql_error(conn));
   }
   result = mysql_store_result(conn);
   mysql_free_result(result);
}

void init_file(qq information,int conn_fd)
{
   MYSQL_RES * result;
   result = mysql_store_result(conn);
   mysql_free_result(result);
   qq information2;
   char query1[200];

   information2.flag = 16;
   strcpy(information2.number,information.number);
   strcpy(information2.to,information.to);
   //让自己发送的时候可以获得当前文件和想要发送的人
   sprintf(query1,"select friends_%s from where friends_number = '%s';",information.number,information.to);
  
   int t = mysql_real_query(conn,query1,strlen(query1));//执行这条mysql 在我的好友表中查询这个人的内容
   if(t != 0)
   {  
       printf("%s\n",mysql_error(conn));
       my_err("select",__LINE__);
   }
  result = mysql_store_result(conn);//获取结果集
  if(result == NULL)
  {
       information2.state = -5;
  }
  mysql_free_result(result);//释放结果集
  send(conn_fd,&information2,sizeof(qq),0);
}


void send_file(qq information,int conn_fd)//转发文件
{
    MYSQL_RES * result;
    result = mysql_store_result(conn);
    mysql_free_result(result);
    int fd ,t ;
    char query1[150] ;
    memset(query1,0,sizeof(query1));
    sprintf(query1 ,"insert into chat_record(my_number,friend_number,record) values(%d,%d, 'send file :%s');",information.number ,information.to,information.record ) ; //加入私聊记录
    if(information.state ==  1)     //如果没有出现异常状态直接转发
    {
        t= mysql_real_query(conn,query1,strlen(query1));
        if(t != 0 )  
        {
             my_err("server mysql_real_query",__LINE__);
             printf("%s\n",mysql_error(conn));
        }
        if( (fd = search(head,information.to) ))   //对方也在线
        information.state = -6;
           send(fd,&information,sizeof(qq),0)  ;
    }
    else if( (fd = search(head,information.to) ))   //对方也在线
    {
        information.state = -6;
        send(fd,&information,sizeof(qq),0)  ;
    }
}


struct chatting * init()
{
    head = (struct chatting *)malloc(sizeof(struct chatting));
    head->next =NULL;
    return head;
}

struct chatting * add(struct chatting * head,qq information,int conn_fd)
{       
    
    struct chatting *ptemp;
    ptemp = head;

    if(head ->next !=NULL)
    {
         while(ptemp ->next != NULL)
        {
            if(information.number == ptemp ->QQ)
            {
               return head;
            }
            ptemp = ptemp -> next;
        }
    }

    struct chatting *new_number;
    new_number = (struct chatting *)malloc(sizeof(struct chatting));
    strcpy(new_number->QQ,information.number);
    strcpy(new_number->username,information.username);
  //  printf("conn_fd %d\n",conn_fd);
    new_number->send_conn_fd = conn_fd;

    ptemp->next = new_number;
    new_number->next = NULL;
    return  head;
}

void setnoblocking(int sock) //设置非阻塞
{
    int opts;
    opts = fcntl(sock, F_GETFL);//获得文件打开方式
    if (opts < 0)
    {
        perror("fcntl(sock,GETFL)");
        exit(1);
    }
    opts = opts | O_NONBLOCK;//设置为非阻塞
    if (fcntl(sock, F_SETFL, opts) < 0)//设置文件属性
    {
        perror("fcntl(sock,SETFL)");
        exit(1);
    }
}


void off_onlie(qq information)
{
    char query1[200];
    char query2[200];
    MYSQL_RES * result;
    MYSQL_RES * my_result;
    MYSQL_ROW row;
    result =mysql_store_result(conn);
    mysql_free_result(result);
    sprintf(query1,"select friends_number from friends_%s;",information.number);
    int t,y=0;
    t = mysql_real_query(conn,query1,strlen(query1));
    //获取函数的结果集

    result = mysql_store_result(conn);
    if(result == NULL)
    return ;
    //在这个循环中，每一次mysql_fetch_row()都获得当前行数据库，并赋值给数组$row，然后自动滑向下一行；在取出最后一行后，函数将返回false，
   // 循环结束。就可以把结果集中的所有数据逐行取出并显示。 
    //获取行和列中的数据
     while(row = mysql_fetch_row(result))
    {
        for(t = 0;t < mysql_num_fields(result);t++)
        {
            sprintf(query2,"update friends_%s set state = 0  where friends_number='%s' ; ",row[t],information.number);
            y = mysql_real_query(conn,query2,strlen(query2));
            if(y != 0)
            {
                my_err("select",__LINE__);
            }

            my_result = mysql_store_result(conn);
            mysql_free_result(my_result);
        }    
    }
}


int *intput(void *arg)
{

    int conn_fd = *(int *)arg;
    qq information;
    int t;
    t = recv(conn_fd, &information, sizeof(qq), 0);
    printf("t %d\n",t);
    printf("conn_fd %d \n",conn_fd);
    //在这里根据recv的返回值来进行登陆状态的改变
    // if(t == -1)
    // {
    //     close(conn_fd);s
    // }
    // if( t == 0 )
    // {
    //     close(conn_fd);
    //     off_onlie(information);//更新好友列表
    // }
    switch (information.flag)
    {
    //  case 0:
    //         off_onlie(information);//更新好友列表
    //         close(conn_fd);
    //         break;
     case 1:
            regist(information, conn_fd);//注册
            break;
     case 2:
            login(information, conn_fd);//好友之间的登陆
            break;
     case 3:
            chat_input(information,conn_fd);//好友之间的聊天
            break;
     case 4:
            chat_friend(information,conn_fd);//添加好友
            break;
     case 5:
            chat_list(information,conn_fd);//好友列表
            break;
     case 6:
            chat_delete(information,conn_fd);//删除好友
            break;
     case 7:
            chat_black(information,conn_fd);//黑名单
            break;
     case 8:
             create_group(information,conn_fd);//创建群聊
             break;
     case 9:
             group_delete(information,conn_fd);//解散群
             break;
     case 10:
             group_list(information,conn_fd);//查看群列表
             break;
     case 11:
             group_member_list(information,conn_fd);;//进入群聊//包括设置管理员权限和一对多聊天//邀请人,踢人
             break;
     case 12:
             group_set_guanli(information,conn_fd);//设置群聊的管理员
             break;
     case 13:
             group_inivt_mebmer(information,conn_fd);//邀请群成员进入群
             break;
     case 14:
             group_kick_out_members(information,conn_fd);//踢人
             break;
     case 15:
            chatting_group(information,conn_fd);//进入群聊
            break;
     case 16:
            init_file(information,conn_fd);//查询有没有这个人
            break;
     case 17:
            send_file(information,conn_fd);//给客户端发送消息
            break;
     case 18:
            look_offline_news(information,conn_fd);//查找离线消息
            break;
     case 19:
            chatting_news(information,conn_fd);//查找好友的消息
            break;       
     case 20:
            group_news(information,conn_fd);//查找群的消息
            break;
     default:
            break;
    }
}

//查询完说明已经读取，读取之后则销毁
///每次离线消息应该是没有读取的
//在离线消息中查找所有的内容，之后在进行清空
void look_offline_news(qq information,int conn_fd)
{

    char query1[200];
    char query2[200];
    MYSQL_RES * result;
    MYSQL_ROW row;
    int y;
    qq server_information;
    sprintf(query1,"select come from off_line_record_%s where come = '%s'; ",information.number,information.number);
    sprintf(query2,"delect from off_line_record_%s ; ",information.number);

    int t = mysql_real_query(conn,query1,strlen(query1));
    if(t != 0)
    {
        my_err("select",__LINE__);
        printf("%s\n",mysql_error(conn));
    }
    result = mysql_store_result(conn);
    //row相当于查询了结果中的一行内容
    //相当于一个二维数组
    //进行一次循环之后会自动的移动到下一列中
    while(row = mysql_fetch_row(result))
   {
       //返回结果集中的列数
          for(y = 0;y < mysql_num_fields(result);y++)   
          {
             strcpy(server_information.to,row[0]);
             strcpy(server_information.number,row[1]); 
             strcpy(server_information.record,row[2]);
             server_information.flag = 18;
             send(conn_fd,&server_information,sizeof(qq),0);
             //发送给客户端
          }
   }
   //发完就清空,方便下次记录
   t = mysql_real_query(conn,query2,strlen(query2));
   if(t != 0 )
   {
       printf("%s\n",mysql_error(conn));
       my_err("server mysql_real_query",__LINE__);
   }
   return ;
}


void chatting_group(qq information,int conn_fd)
{
   MYSQL_RES * result;
   MYSQL_ROW row;
   qq information2;
   information2.flag = 15;
   strcpy(information2.record,information.record);

   result = mysql_store_result(conn);
   mysql_free_result(result);
   
   char query1[200];
   char query2[200];
   
   sprintf(query1,"select group_number from group_%s_%s ; ",information.username,information.number);
   sprintf(query2,"insert into group_%s (group_name,content) values('%s','%s');",information.username,information.record);
   int t = mysql_real_query(conn,query1,sizeof(query1));
   if(t != 0)
   {
       my_err("select",__LINE__);
       printf("%s\n",mysql_error(conn));
   }
   result = mysql_store_result(conn);
   mysql_free_result(result);
   t = mysql_real_query(conn,query1,sizeof(query1));
   int y,fd;
   if(t != 0)
   {
       my_err("select",__LINE__);
       printf("%s\n",mysql_error(conn));
   }
    result = mysql_store_result(conn);
    while(row = mysql_fetch_row(result))
   {
       for(y = 0;y < mysql_num_fields(result); y++)
      { 
            fd = search(head,information.to);
            if(fd == 0)
                add_offline_news(information,information.number,information.to);
            else
                send(fd,&information2,sizeof(qq),0);     
      }
    }
}

void group_kick_out_members(qq information,int conn_fd)
{
   
        MYSQL_RES * result; 
        MYSQL_ROW row;
        result = mysql_store_result(conn);
        mysql_free_result(result);

        char query1[200];
        char query2[200];
        char query3[200];
        int t;
        qq information2;
        information2.flag = 14;
        strcpy(information2.number,information.number);
            
        

        sprintf(query1,"select member_number from group_%s_%s where member_number = '%s' ;",information.username,information.number,information.to);
        t = mysql_real_query(conn,query1,strlen(query1));
        if(t != 0)
        {
            my_err("select",__LINE__);
            printf("%s\n",mysql_error(conn));
        }  
        result = mysql_store_result(conn);
        row = mysql_fetch_row(result);
        if(row = NULL)
        {
            information2.state = -2;
            send(conn_fd,&information2,sizeof(qq),0);
            return ;
        }
        mysql_free_result(result);
        sprintf(query3,"select member_state from group_%s_%s where member_number= '%s' and member_state = 3;",information.username,information.number,information.to);
        t = mysql_real_query(conn,query3,strlen(query3));
        if(t != 0)
        {
            my_err("select",__LINE__);
            printf("%s\n",mysql_error(conn));
        }  
        result = mysql_store_result(conn);
        row = mysql_fetch_row(result);
        if(row == NULL)
        {
          information2.state = 1;
          mysql_free_result(result);
          sprintf(query2,"delete from group_%s_%s where member_number = '%s' ;",information.username,information.number,information.to);
          t = mysql_real_query(conn,query2,strlen(query2));
          if(t != 0)
          {
              my_err("select",__LINE__);
               printf("%s\n",mysql_error(conn));
          }
          result = mysql_store_result(conn);
          mysql_free_result(result);
        }
        else
        {
            information2.state = 3;
        }
      
   
        send(conn_fd,&information2,sizeof(qq),0);
}

//邀请新的群成员加入群
//邀请进入之后，通知本群之中的所有人
void group_inivt_mebmer(qq information, int conn_fd)
{
   MYSQL_RES  * result;
   result = mysql_store_result(conn);
   mysql_free_result(result);
    
   char query1[200];
  // char mebmer[200];
   int fd,t;
   qq information2;
   information2.flag = 13;
   sprintf(information2.record,"用户%s 邀请您加入群聊%s\n",information.number,information.username);
   fd = search(head,information.to);
   strcpy(information2.to,information.to);
   strcpy(information2.number,information.number);
   information.state = 1;
   send(fd,&information2,sizeof(qq),0);
  
   recv(fd,&information2,sizeof(qq),0);
   sprintf(query1,"insert into group_%s_%s (member_number,member_state)  values('%s','%s');",information.username,information.number,information.to,information.state);
   t = mysql_real_query(conn,query1,strlen(query1));
   if(t != 0)
   {
       printf("%s \n ",mysql_error(conn));
   }

   result = mysql_store_result(conn);
   mysql_free_result(result);
   memset(query1,0,sizeof(query1));

   sprintf(query1,"insert into group_%s_%s (member_name) select users_name from users where users_number='%s' ;",information.username,information.number,information.to);
   t = mysql_real_query(conn,query1,strlen(query1));
   if(t != 0)
   {
       information2.state = -1;
       printf("%s \n ",mysql_error(conn));
   }

   result = mysql_store_result(conn);
   mysql_free_result(result);
   send(conn_fd,&information2,sizeof(qq),0);
}

//设置管理员
void group_set_guanli(qq  information,int conn_fd)
{
    MYSQL_RES * result;
    result = mysql_store_result(conn);
    mysql_free_result(result);
    char query1[200];
    char query2[200];
    //查询这个表中有没有成员
    qq information2;
    information2.flag = 12 ;
    sprintf(query1,"select member_number from group_%s_%s where member_number ='%s';",information.username,information.number,information.to);
    int t;
    t = mysql_real_query(conn,query1,strlen(query1));
    printf("t %d \n",t);
    if(t !=0)
    {
        //在这里使用mysql_error 这个报错，最好不要执行strerror(errno)
        printf(" %s\n",mysql_error(conn));
    }
    result = mysql_store_result(conn);
    if(result == NULL)
    {
        information.state = -4;
    }
    else
    {
        mysql_free_result(result);
        sprintf(query2,"update group_%s_%s  set member_state = 4 where member_number = '%s' ;",information.username,information.number,information.to);
        t = mysql_real_query(conn,query2,strlen(query2));  
       if(t != 0)
       {
           my_err("mysql",__LINE__);
       }
        result = mysql_store_result(conn);
        mysql_free_result(result);
    }
    send(conn_fd,&information2,sizeof(qq),0);
}


//查看群成员信息
void group_member_list(qq information,int conn_fd)
{
   MYSQL_RES * result;
   MYSQL_ROW row;
   char query1[100];
   qq information2;
   
   result = mysql_store_result(conn);
   mysql_free_result(result);
   strcpy(information2.number,information.number);
   information2.flag = 11;
   sprintf(query1,"select * from  group_%s_%s ;",information.username,information.number);
   printf("query1   %s \n",query1);
 
   int t,y=0;
   t = mysql_real_query(conn,query1,strlen(query1));
   printf("t %d \n",t);
   if(t !=0)
   {
      //在这里使用mysql_error 这个报错，最好不要执行strerror(errno)
      printf(" %s\n",mysql_error(conn));
   }
   //获取函数的结果集

   result = mysql_store_result(conn);

   if(result == NULL)
   {
       printf("此群不存在!\n");
       return ;
   }
   //获取行和列中的数据
   while(row = mysql_fetch_row(result))
   {
        y = 0;
        while(y != mysql_num_fields(result))
      { 
        if(y == 0)
        {
           if(row[y] == NULL)
            {
                y++;
                continue;
            }
            strcpy(information2.number,row[y]);
            printf("row %s \n",row[y]);
            y++;
        }
        if( y == 1)
        {
              if(row[y] == NULL)
            {
                y++;
                continue;
            }
            strcpy(information2.username,row[y]);
            printf("row %s \n",row[y]);
            y++;
        }
        if(y == 2)
        {
             if(row[y] == NULL)
            {
                y++;
                continue;
            }
            //以整形的形式将row写入到information2.state
            sscanf(row[y],"%d",&information2.state);
            printf("row %s \n",row[y]);
            y++;
        }
      }
         send(conn_fd,&information2,sizeof(qq),0);
    }
}


void group_delete(qq information,int conn_fd)//解散群
{
     char query1[200];
     char query2[200];
     int t;
     MYSQL_RES * result;

     result = mysql_store_result(conn);
     mysql_free_result(result);
    
     sprintf(query2,"delete from group_%s where group_name = '%s';",information.number,information.username);
     t = mysql_real_query(conn,query2,strlen(query2));
     printf("query2 %s \n",query2);
     printf("t %d \n",t);
     if(t !=0)
    {
       //在这里使用mysql_error 这个报错，最好不要执行strerror(errno)
       printf(" %s\n",mysql_error(conn));
     }
     result = mysql_store_result(conn);
     mysql_free_result(result);

     sprintf(query1,"drop table group_%s_%s ;",information.username,information.number);
     t = mysql_real_query(conn,query1,strlen(query1));
     printf("query1 %s \n ",query1);
     printf("t %d \n",t);
     if(t !=0)
    {
       //在这里使用mysql_error 这个报错，最好不要执行strerror(errno)
       printf(" %s\n",mysql_error(conn));
     }
     result = mysql_store_result(conn);
     mysql_free_result(result);
}


void group_list(qq information,int conn_fd)
{

   MYSQL_RES * result;
   MYSQL_ROW row;
   char query1[100];
   qq information2;
   information2.flag = 10;

   sprintf(query1,"select group_name from group_%s;",information.number);
   printf("query1   %s \n",query1);
   result = mysql_store_result(conn);
   mysql_free_result(result);
   int t,y=0;
   t = mysql_real_query(conn,query1,strlen(query1));
   printf("t %d \n",t);
   if(t !=0)
   {
      //在这里使用mysql_error 这个报错，最好不要执行strerror(errno)
      printf(" %s\n",mysql_error(conn));
   }
   //获取函数的结果集

   result = mysql_store_result(conn);

   if(result == NULL)
   {
       strcpy(information2.record,"您没有加入任何的群");
       send(conn_fd,&information2,sizeof(qq),0);
   }
   //获取行和列中的数据
    while(row = mysql_fetch_row(result))
    {
        y=0;
        while(y != mysql_num_fields(result))
      { 
        if(row[y] == NULL)
        {
            y++;
            continue;
        }
         strcpy(information2.record,row[y]);
         printf("row[y] %s\n",row[y]);
         y++;
      }
      send(conn_fd,&information2,sizeof(qq),0);
    }
}


void create_group(qq information,int conn_fd)
{
   char  query1[200]; 
   char query2[200];
   char query3[200];
   char query4[200];
   int t;
   MYSQL_RES  * result;
   result = mysql_store_result(conn);
   mysql_free_result(result);
   //创建一张群的表
  if(information.size != 100)
 {
    sprintf(query1,"create table group_%s_%s (`member_number`VARCHAR(100),`member_name` VARCHAR(100),`member_state` VARCHAR(100))ENGINE=InnoDB DEFAULT CHARSET=utf8;",information.username,information.number);
    t = mysql_real_query(conn,query1,strlen(query1));
   if(t != 0)
   {
       printf("%s \n ",mysql_error(conn));
   }
   result = mysql_store_result(conn);
   mysql_free_result(result);
   information.state = 3;//将创建的人设置为群主
   sprintf(query3,"insert into group_%s_%s (member_number,member_state)  values('%s','%d');",information.username,information.number,information.number,information.state);
   t = mysql_real_query(conn,query3,strlen(query3));
   if(t != 0)
   {
       printf("%s \n ",mysql_error(conn));
   }
   result = mysql_store_result(conn);
   mysql_free_result(result);
   memset(query3,0,sizeof(query3));
   sprintf(query3,"update group_%s_%s set  member_name = ( select users_name from users where users_number='%s') where member_number='%s';",information.username,information.number,information.number,information.number);
   t = mysql_real_query(conn,query3,strlen(query3));
   if(t != 0)
   {
       printf("%s \n ",mysql_error(conn));
   }
   result = mysql_store_result(conn);
   mysql_free_result(result);
 }
   while(1)
   {
      recv(conn_fd,&information,sizeof(qq),0);
      printf("to %s\n",information.to);
      printf("username %s \n",information.username);
      if(strcmp(information.to,"quit") == 0)
      {
          break;
      }
      memset(query2,0,sizeof(query2));
      sprintf(query2,"insert into group_%s_%s (member_number,member_state)  values('%s','%d');",information.username,information.number,information.to,information.state);
      printf("query2 %s\n",query2);
      t = mysql_real_query(conn,query2,strlen(query2));
      result = mysql_store_result(conn);
      mysql_free_result(result);
      if(t != 0)
      {
        printf("%s \n ",mysql_error(conn));
      }
      memset(query2,0,sizeof(query2));
      sprintf(query2,"update group_%s_%s set  member_name = ( select users_name from users where users_number='%s') where member_number='%s';",information.username,information.number,information.to,information.to);
      printf("query2 %s\n",query2);
      t = mysql_real_query(conn,query2,strlen(query2));
      if(t != 0)
      {
        printf("%s \n ",mysql_error(conn));
      }
        result = mysql_store_result(conn);
        mysql_free_result(result);
    }
    sprintf(query4,"insert into group_%s (group_name)   values('%s');",information.number,information.username);
    t = mysql_real_query(conn,query4,strlen(query4));
    printf("query4 %s\n",query4);
    result = mysql_store_result(conn);
    mysql_free_result(result);
    if(t != 0)
   {
       printf("%s \n ",mysql_error(conn));
   }  
}


//黑名单无法聊天，通过链表来进行实现
void chat_black(qq information,int conn_fd)
{
    char query1[200];
    int t;
    MYSQL_RES * result;
    result = mysql_store_result(conn);
    mysql_free_result(result);
    sprintf(query1,"update friends_%s set friends_state=2 where friends_number='%s';",information.number,information.to);

    t = mysql_real_query(conn,query1,strlen(query1));
    printf("t %d\n",t);
    if(t != 0)
    {
        printf("%s\n",mysql_error(conn));
    }
}


//把好友在自己的表中删除
//send传输内容的话不能使用int 类型 必须得是chat *类型
//在这里最好使用mysql 中自带的错误判断语句
void chat_delete(qq information,int conn_fd)
{
   MYSQL_RES * result;
   char query1[150];
   char query2[150];
   int  t=0;
   qq information2;
   information2.flag = 6;
   strcpy(information2.number,information.number);

   result = mysql_store_result(conn);
   mysql_free_result(result);
   sprintf(query1,"delete from friends_%s where friends_number = '%s' ;",information.number,information.to);
   t = mysql_real_query(conn,query1,strlen(query1));
   printf("query1 %s\n",query1);
   printf("t %d\n",t);
   if(t!=0)
   {
       information2.state = -3;
       printf("%s\n",mysql_error(conn));
   }
   result = mysql_store_result(conn);
   mysql_free_result(result);
   sprintf(query2,"delete from friends_%s where friends_number = '%s' ;",information.to,information.number);
   t = mysql_real_query(conn,query2,strlen(query2));
   printf("query2 %s\n",query2);
   printf("t %d\n",t);
   if(t!=0)
   {
       information2.state = -3;
       printf("%s\n",mysql_error(conn));
   }
   result = mysql_store_result(conn);
   mysql_free_result(result);
   send(conn_fd,&information2,sizeof(qq),0);
}


///不能直接返回结果集给客户端
void chat_list(qq information,int conn_fd)//从数据库中获得内容
{
   MYSQL_RES * result;
   MYSQL_ROW row;
   char query1[100];
   qq information2;
   result = mysql_store_result(conn);
   mysql_free_result(result);

   information2.flag = 5;
   strcpy(information2.number,information.number);
   sprintf(query1,"select * from friends_%s;",information.number);
   printf("query1   %s \n",query1);

   int t,y=0;
   t = mysql_real_query(conn,query1,strlen(query1));
   printf("t %d \n",t);
   if(t !=0)
   {
      //在这里使用mysql_error 这个报错，最好不要执行strerror(errno)
      printf(" %s\n",mysql_error(conn));
   }
   //获取函数的结果集

   result = mysql_store_result(conn);

   if(result == NULL)
   {
       strcpy(information2.record,"表中暂无数据");
       send(conn_fd,&information2,sizeof(qq),0);
       return ;
   }
   //获取行和列中的数据
    while(row = mysql_fetch_row(result))
    {
        y = 1;
        while(y != mysql_num_fields(result))
      { 
        if(y == 1)
        {
           if(row[y] == NULL)
            {
                strcpy(information2.to," ");
                y++;
                continue;
            }
            strcpy(information2.to,row[y]);
            printf("row %s \n",row[y]);
            y++;
        }
        if( y == 2)
        {
              if(row[y] == NULL)
            {
                strcpy(information2.username," ");
                y++;
                continue;
            }
            strcpy(information2.username,row[y]);
            printf("row %s \n",row[y]);
            y++;
        }
        if(y == 3)
        {
             if(row[y] == NULL)
            {
                information2.state = 0;
                y++;
                continue;
            }
            sscanf(row[y],"%d",&information2.state);
            printf("row %s \n",row[y]);
            y++;
        }
      }
      send(conn_fd,&information2,sizeof(qq),0);
    }
    /*strcpy(chatting_use.name,"quit");
    strcpy(chatting_use.number,"0");
    send(conn_fd,&chatting_use,sizeof(list),0); */
}




//将好友请求发送给对方，
void chat_friend(qq information,int conn_fd)
{
    char query1[230];
    char query2[230];
    char name[32];
    MYSQL_RES *result;//mysql的结果集
    qq information2;
    //使用另外一个发，不然会影响我当前的结构踢
    int t = 0;
    int fd;

    if(information.state != -8 && information.state != -7)
    {    information2.flag = 4;
        information2.state = 1;
        strcpy(information2.number,information.number);
        strcpy(information2.to,information.to);
        //在这里使用sprintf 获取发送好友请求的字符串
        printf("number %s to %s\n",information.number,information.to);
        sprintf(information2.record,"用户 %s 请求添加您为好友",information.number);
        printf("agree %s\n",information2.record);
        fd = search(head,information.to);
        printf("fd %d\n",fd);
        send(fd,&information2,sizeof(qq),0);//
        
        int ret = recv(conn_fd,&information,sizeof(qq),0);
        printf("state %d\n",information.state);
        printf("number %s\n",information.number);
        if(ret <= 0 )
        {
            printf("recv ret = %d , error = %s\n", ret, strerror(errno));
        }
    }
    if(information.state == -8)//同时将两个用户添加给对方的表中
    {
           printf("----------------------------\n");
           sprintf(query1,"insert into friends_%s (friends_name) select users_name from users where users_number='%s' ;",information.to,information.number);
            printf("query 1 %s\n",query1);
            t = mysql_real_query(conn,query1,strlen(query1));
            if(t!=0)
            {
                printf(" %s\n",strerror(errno));
            }
            printf("-------------\n");
            result = mysql_store_result(conn);
            mysql_free_result(result);
            memset(query1,0,sizeof(query1));
            //mysql update 和select 混用
            sprintf(query1,"update friends_%s set friends_number ='%s',friends_state='1'  where friends_name = (select users_name from users where users_number='%s' );",information.to,information.number,information.number);
            printf("query 1 %s\n",query1);
            t = mysql_real_query(conn,query1,strlen(query1));
            if(t!=0)
            {
                printf(" %s\n",strerror(errno));
            }
            result = mysql_store_result(conn);
            mysql_free_result(result);
        
            sprintf(query2,"insert into friends_%s (friends_name) select users_name from users where users_number='%s' ;",information.number,information.to);
            t = mysql_real_query(conn,query2,strlen(query2));
                if(t!=0)
            {  
                printf(" %s\n",strerror(errno));
            }
            printf("query 2 %s\n",query2);
            result = mysql_store_result(conn);
            mysql_free_result(result);
            memset(query2,0,sizeof(query2));
            sprintf(query2,"update friends_%s set friends_number ='%s',friends_state='1'  where friends_name = (select users_name from users where users_number='%s' );",information.number,information.to,information.to);
            printf("query 2 %s\n",query2);
            t = mysql_real_query(conn,query2,strlen(query2));
            if(t!=0)
            {  
                printf(" %s\n",mysql_error(conn));
            }
            result = mysql_store_result(conn);
            mysql_free_result(result);
            information2.state = -8;
            information2.flag = 4;
            strcpy(information2.number,information.number);
            strcpy(information2.to,information.to);
            strcpy(information2.record,"对方已经同意你的申请");
            conn_fd = search(head,information2.number);
            send(conn_fd,&information2,sizeof(qq),0);
    }
     if(information.state == -7)
    {
            information2.state = -7;
            information2.flag =4;
            strcpy(information2.number,information.number);
            strcpy(information2.to,information.to);
            strcpy(information2.record,"对方已经拒绝你的添加");
            send(conn_fd,&information2,sizeof(qq),0);
    }
}


int  search(struct chatting * head,char * to)
{
    struct chatting * temp;
    int send_fd = 0;
    temp = head->next;
    printf("to %s \n",to);
    while(temp!= NULL)
    {
         printf("temp %d \n",temp->send_conn_fd);
         if(strcmp(temp->QQ,to) == 0)
         {
            if(temp->state == 2)
            {
               return 0;
            }
            else
            {
                send_fd = temp->send_conn_fd;
                break;
            }
         }
         temp = temp ->next;
    }
    //printf("send_conn_fd %d\n",send_fd);
    return send_fd;
}


void chat_input(qq information,int conn_fd)
{
        MYSQL_RES *result;
        MYSQL_ROW row;
        result = mysql_store_result(conn);
        mysql_free_result(result);

        char query1[200];
        char query2[200];
        char query3[200];
        int t,fd;
        char to[13];
        qq information2;
        information2.flag = 3;
        printf("connn_fd %d\n",conn_fd);
        printf("chat _ input flag %d\n",information.flag);
        if(strcmp(information.record," ") == 0 || strcmp(information.record,"登陆成功") == 0)
        {
              return ;
        }
        sprintf(query3,"select friends_state from friends_%s where friends_number = '%s' and friends_state = 2 ; ",information.number,information.to);
        t = mysql_real_query(conn,query3,strlen(query3));
        result = mysql_store_result(conn);
        row = mysql_fetch_row(result);
        if(row != NULL)
        {
           strcpy(information2.record,"对方已经把你拉黑，无法通信");
           information2.state = -2;
           send(conn_fd,&information2,sizeof(qq),0);
           return ;
        }
         
        
        strcpy(information2.number,information.number);
        strcpy(information2.to,information.to);
        strcpy(information2.record,information.record);
        mysql_free_result(result);

        sprintf(query2,"select friends_number from friends_%s where friends_number = '%s' ;",information.number,information.to);
        t = mysql_real_query(conn,query2,strlen(query2));
        result = mysql_store_result(conn);
        row =mysql_fetch_row(result);
        if(row == NULL)
        {
           strcpy(information2.record,"他不是你的好友,无法发送消息");
           information2.state = -1;
           send(conn_fd,&information2,sizeof(qq),0);
           return ;
        }
      
        strcpy(information2.record,information.record);
        mysql_free_result(result);

        printf("record %s\n",information.record);
        printf("intput_record %s\n",information2.record);
        memset(query1,0,150);
        sprintf(query1 ,"insert into chat_record  (my_number,friend_number,record) values('%s','%s','%s');",information.number,information.to,information.record) ; //加入私聊记录
        t = mysql_real_query(conn,query1,strlen(query1));
        if(t!=0)
        {
            printf("添加消息记录失败");  
        }
        strcpy(to,information.to);
        fd = search(head,to);
        printf("-----------------------------------fd %d\n",fd);
         if(fd == 0)
            {
                add_offline_news(information,information.number,information.to);
            }
            else
            {
                    send(fd,&information2,sizeof(qq),0);
            }    
}


void regist(qq information, int conn_fd) //每个用户都需要创建一个自己的表,同时操作应该保留在日志中
{

    char *mysql_query1, *mysql_query2, *mysql_query3,*mysql_query4;
    //一个存入，一个创建每个用户的表，作以联系，发送给日志，存到日志之中
    MYSQL_RES *result;//mysql的结果集
    mysql_query1 = (char *)malloc(300);
    mysql_query2 = (char *)malloc(300);
    mysql_query3 = (char *)malloc(300);
    mysql_query4 = (char *)malloc(300);
    memset(mysql_query1, 0, 300);
    memset(mysql_query2, 0, 300);
    memset(mysql_query3, 0, 300);
    memset(mysql_query4, 0, 300);
    sprintf(mysql_query1, "insert into users(users_number,users_passwd,users_name,users_qusetion,users_answer) values('%s',md5('%s'),'%s','%s','%s');", information.number, information.passwd, information.username, information.qusetion, information.answer);
    printf("mysql_query1 %s\n", mysql_query1);
    int t = mysql_real_query(conn, mysql_query1, strlen(mysql_query1));
    result = mysql_store_result(conn);//API并不会主动释放句柄中的内容，为了节省内存,防止下次读入的时候发生错误
    printf("resigter t %d\n",t); 
    if (t != 0)
    {
        strcpy(information.record,"账号已经存在,请重新申请!");
        send(conn_fd,&information,sizeof(qq), 0);
    }
    else
    {
        mysql_free_result(result);
       // printf("number %s\n",information.number);
        sprintf(mysql_query2, "create table friends_%s (`friends_id` INT UNSIGNED AUTO_INCREMENT,`friends_number`VARCHAR(100),`friends_name` VARCHAR(100),`friends_state` VARCHAR(100),PRIMARY KEY ( `friends_id` ))ENGINE=InnoDB DEFAULT CHARSET=utf8;", information.number);
       // printf("mysql_query2 %s\n",mysql_query2);
        t = mysql_real_query(conn, mysql_query2, strlen(mysql_query2));
        result = mysql_store_result(conn);//API并不会主要释放句柄中的内容
        mysql_free_result(result);
        if(t != 0)
        {
             printf("error 2  t %d\n", t);
        }
        sprintf(mysql_query3, "insert into system_log (user_QQ,record )   values( %s,'regist  a account ');", information.number);
      //  printf("mysql_query3 %s\n",mysql_query3);
        t = mysql_real_query(conn, mysql_query3, strlen(mysql_query3));
        if(t != 0)
        {
            printf("%s\n",mysql_error);
        }
        result = mysql_store_result(conn);
        mysql_free_result(result);
        sprintf(mysql_query4, "create table group_%s (`group_id` INT UNSIGNED AUTO_INCREMENT,`group_name` VARCHAR(100),`content` text DEFAULT NULL, `create_time` datetime DEFAULT NULL,`update_time` datetime DEFAULT NULL, PRIMARY KEY (`group_id`) ) ENGINE=InnoDB DEFAULT CHARSET=utf8;",information.number);
        t = mysql_real_query(conn, mysql_query4, strlen(mysql_query4));
        if(t != 0)
        {
             printf("error 3  t %d\n", t);
        }
        result = mysql_store_result(conn);
        mysql_free_result(result);
        memset(mysql_query4,0,sizeof(mysql_query4));
        sprintf(mysql_query4,"create table off_line_record_%s( go VARCHAR(100),come VARCHAR(100), record VARCHAR(100), time  TIMESTAMP DEFAULT CURRENT_TIMESTAMP) ;",information.number);
        t = mysql_real_query(conn , mysql_query4,strlen(mysql_query4));
        if(t != 0)
        {
            printf("errro  4 t %s\n",mysql_error(conn));
        }
        result = mysql_store_result(conn);
        mysql_free_result(result);
        strcpy(information.record,"账号申请成功");
        send(conn_fd,&information,sizeof(qq), 0);
    }
    free(mysql_query1);
    free(mysql_query2);
    free(mysql_query3);
    free(mysql_query4);
    return;
}



void login(qq information, int conn_fd)
{
    int fd = 99999 ;
    fd = search(head,information.number);
    printf("fd %d\n",fd);
    if (fd != 0)
    {
       information.state = -2;
       send(conn_fd,&information,sizeof(qq),0);
       return ;
    }
    char *mysql_login, *mysql_passwd, *mysql_log;
    int temp;
    MYSQL_RES *result;//mysql的结果集
    mysql_login = (char *)malloc(200);
    mysql_passwd = (char *)malloc(200);
    mysql_log = (char *)malloc(300);
    memset(mysql_login, 0, 200);
    memset(mysql_passwd, 0, 200);
    memset(mysql_log, 0, 300);
    //在mysql 中搜索如果没有的话返回NULL,
    result = mysql_store_result(conn);
    mysql_free_result(result);
    sprintf(mysql_login, "select users_number from users where users_number like \"%s\";", information.number);
    //printf("mysql_passwd %s\n", mysql_passwd);
    temp = mysql_real_query(conn, mysql_login, strlen(mysql_login)); //判断是不是存在
    result=mysql_store_result(conn);//获取结果集
    printf("temp--------------------------------------------- %d\n", temp);
    if (temp != 0)
    {
        strcpy(information.record,"此用户不存在");
        information.state = -2;
        send(conn_fd, &information, sizeof(qq), 0);
        return;
    }
    else
    {
        mysql_free_result(result);//释放结果集,防止没有覆盖之前的
        sprintf(mysql_passwd, "select users_number,users_passwd from users where users_number='%s' and users_passwd='%s';", information.number, information.passwd);
      //  printf("mysql_passwd %s\n", mysql_passwd);
        temp = mysql_real_query(conn, mysql_passwd, strlen(mysql_passwd));
        printf("temp %d\n", temp); // 1
        result = mysql_store_result(conn);
        if (result != NULL)
        {
            strcpy(information.record,"登陆成功!");
            information.state = 1;


            head=add(head,information,conn_fd);

            send(conn_fd, &information, sizeof(qq), 0);
            sprintf(mysql_log, "insert into system_log (user_QQ,record )   values( %d,'sign in ');", information.number);
            mysql_real_query(conn, mysql_log, strlen(mysql_log));
        }
        else
        {
            strcpy(information.record,"passwd wrong");
            information.state = -1;
            send(conn_fd, &information,sizeof(qq), 0);
        }
    }
    free(mysql_login);
    free(mysql_passwd);
}



int main()
{

    int sock_fd, conn_fd, i;
    int epfd, nfds, optval; //nfds表示事件发生的个数
    socklen_t cli_len;
    pthread_t tid;
    struct sockaddr_in cli_addr, serv_addr;
    //创建一个TCP套接字
    struct epoll_event ev, events[20]; //声明epoll_event结构体变量,ev用于注册事件,数组用于回传要处理的事件
    init();
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0)
    {
        perror("socket");
    }

    optval = 1;
    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, (void *)&optval, sizeof(int)) < 0)
    {
        perror("setsockopt");
    }

    bzero(&serv_addr, sizeof(serv_addr));          //地址清空
    serv_addr.sin_family = AF_INET;                //IPV4的协议族类型
    serv_addr.sin_port = htons(SERV_PORT);         //端口号
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); //可能有多块网卡的情况，使用这个将会绑定在默认的网卡之上即原本的IP地址

    //套接字绑定到本地的端口
    if (bind(sock_fd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr_in)) < 0)
    {
        perror("bind");
    }
    if (listen(sock_fd, LISTENQ) < 0)
    {
        perror("listen");
    }
    //创建epoll语句
    epfd = epoll_create(256);
    ev.data.fd = sock_fd;
    ev.events = EPOLLIN;
    conn = mysql_init(NULL);
    mysql_real_connect(conn, "127.0.0.1", "root", "Lhd013012?", "chatting_usrs", 0, NULL, 0);
    epoll_ctl(epfd, EPOLL_CTL_ADD, sock_fd, &ev); //把监听的套接字添加进注册表中
    signal(SIGPIPE, SIG_IGN);
    cli_len = sizeof(struct sockaddr_in);
    while (1)
    {
        nfds = epoll_wait(epfd, events, 20, 500);
        for (i = 0; i < nfds; ++i)
        {
            //如果检测到一个socket用户连接到了绑定的端口
            if (events[i].data.fd == sock_fd)
            {
                conn_fd = accept(sock_fd, (struct sockaddr *)&cli_addr, &cli_len);
                if (conn_fd < 0)
                {
                    perror("conn_fd");
                }
                printf("accpet ip from %s", inet_ntoa(cli_addr.sin_addr)); //取得客户端的ip

                ev.data.fd = conn_fd;
                ev.events = EPOLLIN;
                epoll_ctl(epfd, EPOLL_CTL_ADD, conn_fd, &ev); //epfd------epoll 句柄
            }
        ///    setnoblocking(conn_fd);         //设置为非阻塞的,防止发送的上层数据大于缓冲区
            if (events[i].events & EPOLLIN) //如果是已经连接的用户，并且收到数据，那么进行读入
            {
                conn_fd = events[i].data.fd;
                if (pthread_create(&tid, NULL, (void *)intput, (void *)&conn_fd) < 0)
                {
                    perror("create pthread failed!");
                }
            }
        }
    }
    mysql_close(conn); //关闭mysql
    close(sock_fd);    //关闭
    return 0;
}
