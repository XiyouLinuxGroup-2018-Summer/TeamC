#include "sql.h"

void* sql_err(int line, MYSQL * _mysql)
{
    fprintf(stderr, "%d: Error: %s\n", line, mysql_error(_mysql));
    return NULL;
}

char* SearchAccId(int id, char * acc)
{
    int ret;
    char sql[128];
    sprintf(sql, "select * from `%s`.`%s` where id = %d;", Data_ChatBase, Table_Userlist, id);
    ret = mysql_real_query(_mysql, sql, strlen(sql));
    if (ret)
        sql_err(__LINE__, _mysql);
    _res = mysql_store_result(_mysql);
    if (_res == NULL)
    {
        if (mysql_field_count(_mysql) == 0)
            return NULL;
        else
            sql_err(__LINE__, _mysql);
    }
    _row = mysql_fetch_row(_res);
    // id [flag] name [flag] passmd5 [flag] question [flag] answer [flag]
    sprintf(acc, "%s%s%s%s%s%s%s%s%s%s", _row[0], _END_, _row[1], _END_, _row[2], _END_, _row[3], _END_, _row[4], _END_);

    mysql_free_result(_res);
    return acc;
}

int LoginAcc(int id, char * pass)
{
    int ret;
    char sql[128];
    sprintf(sql, "select id, passmd5 from `%s`.`user_list` where id = %d AND passmd5 = md5('%s')", Data_ChatBase, id, pass);
    ret = mysql_real_query(_mysql, sql, strlen(sql));
    if (ret)
        sql_err(__LINE__, _mysql);
    _res = mysql_store_result(_mysql);
    if (_res == NULL)                           // 出错或者无数据
    {
        if (mysql_field_count(_mysql) != 0)     // 如果出错
            sql_err(__LINE__, _mysql);
        mysql_free_result(_res);
        return 0;                                   // 结果匹配
    }
    mysql_free_result(_res);
    return 1;                                   // 结果匹配
}

int RegACC(char * name, char * pass, char * problem, char * answer)
{
    int ret, getid;
    char sql[128];
    sprintf(sql, "insert into `%s`.`user_list`(username, passmd5, question, answer) values('%s', md5('%s'), '%s', '%s');", Data_ChatBase, name, pass, problem, answer);
    ret = mysql_real_query(_mysql, sql, strlen(sql));
    if (ret)
        sql_err(__LINE__, _mysql);
    else
    {
        // 释放结果集，以便执行下一条sql语句
        _res = mysql_store_result(_mysql);
        mysql_free_result(_res);
        // SELECT LAST_INSERT_ID(); 基于连接的，线程安全，不会在高并发的时候导致id混乱
        strcpy(sql, "SELECT LAST_INSERT_ID();");
        ret = mysql_real_query(_mysql, sql, strlen(sql));
        if (ret)
            sql_err(__LINE__, _mysql);
        _res = mysql_store_result(_mysql);
        _row = mysql_fetch_row(_res);
        getid = atoi(_row[0]);
        mysql_free_result(_res);    // 释放结果集
    }
    // 如果有旧表，删除
    ret = mysql_real_query(_mysql, DEL_TABLE(Data_UserInfo, getid), strlen(DEL_TABLE(Data_UserInfo, getid)));
    if (ret)
        sql_err(__LINE__, _mysql);
    _res = mysql_store_result(_mysql);
    mysql_free_result(_res);
    // 开始为新用户重新创建表
    mysql_real_query(_mysql, USER_INFO_CRETABLE(Data_UserInfo, getid), strlen(USER_INFO_CRETABLE(Data_UserInfo, getid)));
    _res = mysql_store_result(_mysql);
    mysql_free_result(_res);
    // 为新用户创建一个离线消息记录表
    sprintf(sql, "CREATE TABLE IF NOT EXISTS `%s`.`%d` (  `id` INT UNSIGNED NOT NULL, `status` INT NOT NULL, `msg` VARCHAR(512) NOT NULL, `time` DATETIME NOT NULL);", Data_OfflineMsg, getid);
    ret = mysql_real_query(_mysql, sql, strlen(sql));
    if (ret)
        sql_err(__LINE__, _mysql);
    _res = mysql_store_result(_mysql);
    mysql_free_result(_res);

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
    sprintf(sql, "insert into `%s`.`%d` values(%d, '%s', 0, 0);", Data_UserInfo, user_id, friend_id, friend_name);
    ret = mysql_real_query(_mysql, sql, strlen(sql));
    if (ret)
        sql_err(__LINE__, _mysql);

    _res = mysql_store_result(_mysql);
    mysql_free_result(_res);

    return 1;
}

int ShiFriend(int user_id, int friend_id)
{
    int ret;
    char sql[128];
    sprintf(sql, "update `%s`.`%d` set Shield = 1 where id = %d AND status = 0;", Data_UserInfo, user_id, friend_id);
    ret = mysql_real_query(_mysql, sql, strlen(sql));
    if (ret)
        sql_err(__LINE__, _mysql);
    _res = mysql_store_result(_mysql);
    mysql_free_result(_res);

    return 1;
}

int _DelFriend(int user_id, int friend_id)
{
    int ret;
    char sql[128];
    sprintf(sql, "delete from `%s`.`%d` where id = %d AND status = 0;", Data_UserInfo, user_id, friend_id);
    ret = mysql_real_query(_mysql, sql, strlen(sql));
    if (ret)
        sql_err(__LINE__, _mysql);
    _res = mysql_store_result(_mysql);
    mysql_free_result(_res);
    return 1;
}

int DelFriend(int user_id, int friend_id)
{
    int ret0 = _DelFriend(user_id, friend_id);
    int ret1 = _DelFriend(friend_id, user_id);
    return ret0 | ret1;
}

char** UserRelaList(int user_id, char * reststr[], int flag)
{
    int ret, count = 0, k;
    char sql[128];
    k = flag ? STA_GRP_NOR : STA_FRI_NOR;
    sprintf(sql, "select id, name, Shield from `%s`.`%d` where status = %d order by id;", Data_UserInfo, user_id, k);
    ret = mysql_real_query(_mysql, sql, strlen(sql));
    if (ret)
        sql_err(__LINE__, _mysql);
    _res = mysql_store_result(_mysql);
    if (_res == NULL)
    {
        if (mysql_field_count(_mysql) == 0)     // 无好友列表
            return NULL;
        else                                    // 函数调用失败
            sql_err(__LINE__, _mysql);
    }
    while ((_row = mysql_fetch_row(_res)))
    {
        sprintf(reststr[count++], "%s%s%s%s%s%s\n", _row[0], _END_,  _row[1], _END_, _row[2], _END_);
    }
    mysql_free_result(_res);

    return reststr;
}

int CreateGroup(int owner_id, char * name, char * something)
{
    int ret, groupid;
    char sql[128];
    // 在ChatR_Base中加入内容
    if (something != NULL)
        sprintf(sql, "insert into `%s`.`%s`(name, create_time, member_num, introduction) values ('%s', CURDATE(), 1, '%s');", Data_ChatBase, Table_Grouplist, name, something);
    else
        sprintf(sql, "insert into `%s`.`%s`(name, create_time, member_num) values ('%s', CURDATE(), 1);", Data_ChatBase, Table_Grouplist, name);

    ret = mysql_real_query(_mysql, sql, strlen(sql));
    if (ret)
        sql_err(__LINE__, _mysql);
    _res = mysql_store_result(_mysql);
    mysql_free_result(_res);

    // 获取新建群聊的id
    strcpy(sql, "SELECT LAST_INSERT_ID();");
    ret = mysql_real_query(_mysql, sql, strlen(sql));
    if (ret)
        sql_err(__LINE__, _mysql);
    _res = mysql_store_result(_mysql);
    _row = mysql_fetch_row(_res);
    groupid = atoi(_row[0]);

    // 清除残余表
    ret = mysql_real_query(_mysql, DEL_TABLE(Data_GroupInfo, groupid), strlen(DEL_TABLE(Data_GroupInfo, groupid)));
    if (ret)
        sql_err(__LINE__, _mysql);
    _res = mysql_store_result(_mysql);
    mysql_free_result(_res);

    // 开始为新群建表
    sprintf(sql, "CREATE TABLE IF NOT EXISTS `%s`.`%d` ( `id` INT UNSIGNED NOT NULL, `status` INT UNSIGNED NOT  NULL, PRIMARY KEY(`id`));");
    ret = mysql_real_query(_mysql, sql, strlen(sql));
    if (ret)
        sql_err(__LINE__, _mysql);
    _res = mysql_store_result(_mysql);
    mysql_free_result(_res);
    
    // 开始为新表添加数据
    sprintf(sql, "insert into `%s`.`%d` values(%d, %d);", Data_GroupInfo, groupid, owner_id, GRP_STA_OWN);
    ret = mysql_real_query(_mysql, sql, strlen(sql));
    if (ret)
        sql_err(__LINE__, _mysql);
    _res = mysql_store_result(_mysql);
    mysql_free_result(_res);

    // 更新own_id的关系
    sprintf(sql, "insert into `%s`.`%d` values(%d, '%s', %d, %d);",  Data_UserInfo, owner_id, groupid, STA_GRP_NOR, STA_BE_NORM);
    ret = mysql_real_query(_mysql, sql, strlen(sql));
    if (ret)
        sql_err(__LINE__, _mysql);
    _res = mysql_store_result(_mysql);
    mysql_free_result(_res);

    return 1;
}

char* SearchGrpId(int id, char * grp)
{
    int ret;
    char sql[128];
    sprintf(sql, "select * from `%s`.`%s` where id = %d;", Data_ChatBase, Table_Grouplist, id);
    ret = mysql_real_query(_mysql, sql, strlen(sql));
    if (ret)
        sql_err(__LINE__, _mysql);
    _res = mysql_store_result(_mysql);
    if (_res == NULL)
    {
        if (mysql_field_count(_mysql) == 0)
            return NULL;
        else
            sql_err(__LINE__, _mysql);
    }
    _row = mysql_fetch_row(_res);
    // id [flag] name [flag] create_time [flag] member_num [flag] introduction [flag]
    sprintf(grp, "%s%s%s%s%s%s%s%s%s%s", _row[0], _END_, _row[1], _END_, _row[2], _END_, _row[3], _END_, _row[4], _END_);

    mysql_free_result(_res);
    return grp;
}

int AddOneToGrp(int user_id, char * user_name, int grp_id, char * grp_name)
{
    int ret;
    char sql[128];
    sprintf(sql, "insert into `%s`.`%s` values(%d, '%s', %d);", Data_GroupInfo, grp_id, user_id, user_name, GRP_STA_NOR);
    ret = mysql_real_query(_mysql, sql, strlen(sql));
    if (ret)
        sql_err(__LINE__, _mysql);
    _res = mysql_store_result(_mysql);
    mysql_free_result(_res);
    
    sprintf(sql, "insert into `%s`.`%s` values(%d, '%s', %d, %d);", Data_UserInfo, user_id, grp_id, grp_name, STA_GRP_NOR, STA_BE_NORM);
    ret = mysql_real_query(_mysql, sql, strlen(sql));
    if (ret)
        sql_err(__LINE__, _mysql);
    _res = mysql_store_result(_mysql);
    mysql_free_result(_res);
    
    return 1;
}

int GrpMemberList(int grp_id, int mem_id[MEM_NUM], char mem_name[MEM_NUM][USER_NAME_MAX + 1], int mem_sta[MEM_NUM])
{
    int ret, count = 0;
    char sql[128];
    sprintf(sql, "select id, name, status from `%s`.`%s`;", Data_GroupInfo, grp_id);
    ret = mysql_real_query(_mysql, sql, strlen(sql));
    if (ret)    
        sql_err(__LINE__, _mysql);
    _res = mysql_store_result(_mysql);
    if (_res == NULL)
    {
        if (mysql_field_count(_mysql) == 0)     // 无好友列表
            return NULL;
        else                                    // 函数调用失败
            sql_err(__LINE__, _mysql);
    }
    while ((_row = mysql_fetch_row(_res)))
    {
        if (mem_id)
            mem_id[count] = atoi(_row[0]);
        if (mem_name)
            strcpy(mem_name[count], _row[1]);
        if (mem_sta)
            mem_sta[count] = atoi(_row[2]);
        count++;
    }

    mysql_free_result(_res);
    return 1;
}

int OfflineMSG(Package * msg, int tar_id, int flag)
{
    int ret;
    char sql[1024];
    sprintf(sql, "insert into `%s`.%d` values( %d, %d, %s, now());", Data_OfflineMsg, tar_id, msg->source_id, flag, msg->strmsg);
    ret = mysql_real_query(_mysql, sql, strlen(sql));
    if (ret)
        sql_err(__LINE__, _mysql);
    _res = mysql_store_result(_mysql);

    mysql_free_result(_res);
    return 1;
}
