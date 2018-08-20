#include "base.h"
#include "account.h"

Account* SearchAccId(int id, Account * acc)
{
    int ret;
    char sql[128];
    acc = (Account*)malloc(sizeof(Account));
    if((mysql_real_connect(_mysql, NULL, MS_USER, MS_PASS, Data_ChatBase, 0, NULL, 0)) == NULL)
    {
        fprintf(stderr, "%d: %s\n", __LINE__, mysql_error(_mysql));
        return NULL;
    }
    sprintf(sql, "select * from user_list where id = %d;", id);
    ret = mysql_real_query(_mysql, sql, strlen(sql));
    if (ret)
    {
        fprintf(stderr, "%d: %s\n", __LINE__, mysql_error(_mysql));
        free(acc);
        mysql_close(_mysql);
        return NULL;
    }
    _res = mysql_store_result(_mysql);
    if (_res == NULL)
    {
        if (mysql_field_count(_mysql) == 0)
        {
            free(acc);
            mysql_close(_mysql);
            return NULL;
        }
        else
        {
            fprintf(stderr, "%d: Error: %s\n", __LINE__, mysql_error(_mysql));
            mysql_close(_mysql);
            return NULL;
        }
    }
    _row = mysql_fetch_row(_res);
    acc->id = atoi(_row[0]);
    strcpy(acc->name, _row[1]);
    strcpy(acc->passmd5, _row[2]);
    strcpy(acc->question, _row[3]);
    strcpy(acc->answer, _row[4]);

    mysql_free_result(_res);
    mysql_close(_mysql);
    return acc;
}

void SearchAccNm(char * name, char * res_str)
{
    int ret;
    char sql[128];
    memset(res_str, 0, sizeof(res_str));
    if((mysql_real_connect(_mysql, NULL, MS_USER, MS_PASS, Data_ChatBase, 0, NULL, 0)) == NULL)
    {
        fprintf(stderr, "%d: %s\n", __LINE__, mysql_error(_mysql));
        return ;
    }
    sprintf(sql, "select id, username from  user_list where username = '%s'", name);
    ret = mysql_real_query(_mysql, sql, strlen(sql));
    if (ret)
    {
        fprintf(stderr, "%d: %s\n", __LINE__, mysql_error(_mysql));
        mysql_close(_mysql);
        return ;
    }
    _res = mysql_store_result(_mysql);
    if (_res == NULL)
    {
        if (mysql_field_count(_mysql) == 0)     // 如果无数据
            strcpy(res_str, "\r\t\n");          // 标记给客户端此次未获取数据
        else                                    // 如果出错
        {
            fprintf(stderr, "%d: Error: %s\n", __LINE__, mysql_error(_mysql));
            strcpy(res_str, "\r\t\n");
        }
        mysql_close(_mysql);
        return;
    }
    while ((_row = mysql_fetch_row(_res)))
    {
        char tempstr[128];
        sprintf(tempstr, "[%s] \"%s\"\n", _row[0], _row[1]);
        strcat(res_str, tempstr);
    }
    mysql_free_result(_res);
    mysql_close(_mysql);
}

int LoginAcc(int id, char * pass)
{
    int ret;
    char sql[128];
    if((mysql_real_connect(_mysql, NULL, MS_USER, MS_PASS, Data_ChatBase, 0, NULL, 0)) == NULL)
    {
        fprintf(stderr, "%d: %s\n", __LINE__, mysql_error(_mysql));
        return 0;
    }
    sprintf(sql, "select id, passmd5 from user_list where id = %d AND passmd5 = md5('%s')", id, pass);
    ret = mysql_real_query(_mysql, sql, strlen(sql));
    if (ret)
    {
        fprintf(stderr, "%d: %s\n", __LINE__, mysql_error(_mysql));
        mysql_close(_mysql);
        return 0;
    }
    _res = mysql_store_result(_mysql);
    if (_res == NULL)                           // 出错或者无数据
    {
        if (mysql_field_count(_mysql) != 0)     // 如果出错
            fprintf(stderr, "%d: Error: %s\n", __LINE__, mysql_error(_mysql));
        mysql_close(_mysql);
        return 0;
    }
    mysql_free_result(_res);
    mysql_close(_mysql);
    return 1;                                   // 结果匹配
}

int RegACC(char * name, char * pass, char * problem, char * answer)
{
    int ret, flag = 0, getid;
    char sql[128];
    if (problem == NULL)
        flag = 1;
    if ((mysql_real_connect(_mysql, NULL, MS_USER, MS_PASS, Data_ChatBase, 0, NULL, 0)) == NULL)
    {
        fprintf(stderr, "%d: %s\n", __LINE__, mysql_error(_mysql));
        return 0;
    }
    if (flag)
        sprintf(sql, "insert into user_list(username, passmd5) values('%s', md5('%s'));", name, pass);
    else
        sprintf(sql, "insert into user_list(username, passmd5, question, answer) values('%s', md5('%s'), '%s', '%s');", name, pass, problem, answer);
    ret = mysql_real_query(_mysql, sql, strlen(sql));
    if (ret)
    {
        fprintf(stderr, "%d: %s\n", __LINE__, mysql_error(_mysql));
        mysql_close(_mysql);
        return 0;
    }
    else
    {
        // 释放结果集，以便执行下一条sql语句
        _res = mysql_store_result(_mysql);
        mysql_free_result(_res);
        // SELECT LAST_INSERT_ID(); 基于连接的，线程安全，不会在高并发的时候导致id混乱
        strcpy(sql, "SELECT LAST_INSERT_ID();");
        mysql_real_query(_mysql, sql, strlen(sql));
        _res = mysql_store_result(_mysql);
        _row = mysql_fetch_row(_res);
        getid = atoi(_row[0]);
        mysql_free_result(_res);    // 释放结果集
    }
    // 如果有旧表，删除
    mysql_real_query(_mysql, DEL_TABLE(Data_UserInfo, getid), strlen(DEL_TABLE(Data_UserInfo, getid)));
    _res = mysql_store_result(_mysql);
    mysql_free_result(_res);
    // 开始为新用户重新创建表
    mysql_real_query(_mysql, USER_INFO_CRETABLE(Data_UserInfo, getid), strlen(USER_INFO_CRETABLE(Data_UserInfo, getid)));
    _res = mysql_store_result(_mysql);
    mysql_free_result(_res);
    mysql_close(_mysql);

    return getid;
}

int AddFriend(int user_id, int friend_id, char * user_name, char * friend_name)
{
    int ret0, ret1;
    ret0 = _AddFriend(user_id, friend_id, friend_name);
    ret1 = _AddFriend(friend_id, user_id, user_name);
    return ret0 & ret1;
}

int _AddFriend(int user_id, int friend_id, char * friend_name)
{
    int ret;
    char sql[128];
    if ((mysql_real_connect(_mysql, NULL, MS_USER, MS_PASS, Data_UserInfo, 0, NULL, 0)) == 0)
    {
        fprintf(stderr, "%d: %s\n", __LINE__, mysql_error(_mysql));
        return 0;
    }
    sprintf(sql, "insert into `%s`.`%d` values(%d, '%s', 0, 0);", Table_Userlist, user_id, friend_id, friend_name);
    ret = mysql_real_query(_mysql, sql, strlen(sql));
    if (ret)
    {
        fprintf(stderr, "%d: %s\n", __LINE__, mysql_error(_mysql));
        mysql_close(_mysql);
        return 0;
    }
    mysql_close(_mysql);
    return 1;
}

int ShiFriend(int user_id, int friend_id)
{
    int ret;
    char sql[128];
    if ((mysql_real_connect(_mysql, NULL, MS_USER, MS_PASS, Data_UserInfo, 0, NULL, 0)) == NULL)
    {
        fprintf(stderr, "%d: %s\n", __LINE__, mysql_error(_mysql));
        return 0;
    }
    sprintf(sql, "update `%s`.`%d` set Shield = 1 where id = %d AND status = 0;", Data_UserInfo, user_id, friend_id);
    ret = mysql_real_query(_mysql, sql, strlen(sql));
    if (ret)
    {
        fprintf(stderr, "%d: %s\n", __LINE__, mysql_error(_mysql));
        mysql_close(_mysql);
        return 0;
    }
    mysql_close(_mysql);
    return 1;
}

int _DelFriend(int user_id, int friend_id)
{
    int ret;
    char sql[128];
    if ((mysql_real_connect(_mysql, NULL, MS_USER, MS_PASS, Data_UserInfo, 0, NULL, 0)) == NULL)
    {
        fprintf(stderr, "%d: %s\n", __LINE__, mysql_error(_mysql));
        return 0;
    }
    sprintf(sql, "delete from `%s`.`%d` where id = %d AND status = 0;", Data_UserInfo, user_id, friend_id);
    ret = mysql_real_query(_mysql, sql, strlen(sql));
    if (ret)
    {
        mysql_close(_mysql);
        return 0;
    }
    mysql_close(_mysql);
    return 1;
}

int DelFriend(int user_id, int friend_id)
{
    int ret0 = _DelFriend(user_id, friend_id);
    int ret1 = _DelFriend(friend_id, user_id);
    return ret0 | ret1;
}

char* FriendList(int user_id, char * reststr)
{
    int ret;
    char sql[128];
    if ((mysql_real_connect(_mysql, NULL, MS_USER, MS_PASS, Data_UserInfo, 0, NULL, 0))== NULL)
    {
        fprintf(stderr, "%d: %s\n", __LINE__, mysql_error(_mysql));
        return NULL;
    }
    sprintf(sql, "select id, name, Shield from `%s`.`%d` where status = %d order by id;", Data_UserInfo, user_id, STA_FRI_NOR);
    ret = mysql_real_query(_mysql, sql, strlen(sql));
    if (ret)
    {
        fprintf(stderr, "%d: %s\n", __LINE__, mysql_error(_mysql));
        mysql_close(_mysql);
        return NULL;
    }
    _res = mysql_store_result(_mysql);
    if (_res == NULL)
    {
        if (mysql_field_count(_mysql) == 0)     // 无好友列表
        {
            strcpy(reststr, "\r\t\n");
            mysql_close(_mysql);
            return reststr;
        }
        else                                    // 函数调用失败
        {
            fprintf(stderr, "%d: Error: %s\n", __LINE__, mysql_error(_mysql));
            mysql_close(_mysql);
            return NULL;
        }
    }
    memset(reststr, 0, sizeof(reststr));
    while((_row = mysql_fetch_row(_res)))
    {
        char tempstr[128];                 // id    name    屏蔽状态
        sprintf(tempstr, "[%s][%s][%s]\n", _row[0], _row[1], _row[2]);
        strcat(reststr, tempstr);
    }
    mysql_free_result(_res);
    mysql_close(_mysql);
    return reststr;
}

// 群主id, 群名， 群介绍， 群成员人数， 群mysql_insert_id
// 返回群id
int CreateGroup(int owner_id, char * name, char * createtime, char * something)
{
    int ret, groupid;
    char sql[128];
    if ((mysql_real_connect(_mysql, NULL, MS_USER, MS_PASS, Data_GroupInfo, 0, NULL, 0)) == NULL)
    {
        fprintf(stderr, "%d: %s\n", __LINE__, mysql_error(_mysql));
        return 0;
    }
    // 在ChatR_Base中加入内容
    sprintf(sql, "insert into `ChatR_Base`.`group_list`(name, create_time, member_num, introduction) values ('%s', '%s', 1, '%s');", name, createtime, something);
    ret = mysql_real_query(_mysql, sql, strlen(sql));
    if (ret)
    {
        fprintf(stderr, "%d: %s\n", __LINE__, mysql_error(_mysql));
        mysql_close(_mysql);
        return 0;
    }
    _res = mysql_store_result(_mysql);
    mysql_free_result(_res);
    // 获取新建群聊的id
    strcpy(sql, "SELECT LAST_INSERT_ID();");
    ret = mysql_real_query(_mysql, sql, strlen(sql));
    if (ret)
    {
        fprintf(stderr, "%d: %s\n", __LINE__, mysql_error(_mysql));
        mysql_close(_mysql);
        return 0;
    }
    _res = mysql_store_result(_mysql);
    _row = mysql_fetch_row(_res);
    groupid = atoi(_row[0]);
    // 清除残余表
    ret = mysql_real_query(_mysql, DEL_TABLE(Data_GroupInfo, groupid), strlen(DEL_TABLE(Data_GroupInfo, groupid)));
    if (ret)
    {
        fprintf(stderr, "%d: %s\n", __LINE__, mysql_error(_mysql));
        mysql_close(_mysql);
        return 0;
    }
    _res = mysql_store_result(_mysql);
    mysql_free_result(_res);
    // 开始为新群建表
    sprintf(sql, "CREATE TABLE IF NOT EXISTS `%s`.`%d` ( `id` INT UNSIGNED NOT NULL, `status` INT UNSIGNED NOT  NULL, PRIMARY KEY(`id`));");
    ret = mysql_real_query(_mysql, sql, strlen(sql));
    if (ret)
    {
        fprintf(stderr, "%d: %s\n", __LINE__, mysql_error(_mysql));
        mysql_close(_mysql);
        return 0;
    }
    _res = mysql_store_result(_mysql);
    mysql_free_result(_res);
    // 开始为新表添加数据
    sprintf(sql, "insert into `%s`.`%d` values(%d, %d);", Data_GroupInfo, groupid, owner_id, GRP_STA_OWN);
    ret = mysql_real_query(_mysql, sql, strlen(sql));
    if (ret)
    {
        fprintf(stderr, "%d: %s\n", __LINE__, mysql_error(_mysql));
        mysql_close(_mysql);
        return 0;
    }
    _res = mysql_store_result(_mysql);
    mysql_free_result(_res);
    // 更新own_id的关系
    sprintf(sql, "insert into `%s`.`%d` values(%d, '%s', %d, %d);",  Data_UserInfo, owner_id, groupid, STA_GRP_NOR, STA_BE_NORM);
    ret = mysql_real_query(_mysql, sql, strlen(sql));
    if (ret)
    {
        fprintf(stderr, "%d: %s\n", __LINE__, mysql_error(_mysql));
        mysql_close(_mysql);
        return 0;
    }
    mysql_close(_mysql);
    return 1;
}
