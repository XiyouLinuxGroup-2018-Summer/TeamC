#include "sql.h"
#define DEBUG

extern MYSQL * _mysql;

void* sql_err(int line, MYSQL * mysql)
{
    COLOR(S_DEFAULT_BACK, fRED);  
    BLOD();
    fprintf(stderr, "\n");
    get_time(NULL);
    fprintf(stderr, "%d: Error: %s\n\n", line, mysql_error(mysql));
    CLOSE();
    return NULL;
}

char* SearchAccId(int id, char * name)
{
    int ret;
    char sql[256];
    sprintf(sql, "select username from `%s`.`%s` where id = %d;", Data_ChatBase, Table_Userlist, id);

#ifdef DEBUG
    fprintf(stdout, "\t[数据库] 查询用户: %d\n", id);
    fprintf(stdout, "\tSQL::  %s\n", sql);
#endif

    ret = mysql_real_query(_mysql, sql, strlen(sql));
    if (ret)
        sql_err(__LINE__, _mysql);
    MYSQL_RES * _res = mysql_store_result(_mysql);
    if (_res == NULL)
    {
        #ifdef DEBUG
            fprintf(stdout, "\t>> 查询失败！\n");
        #endif

        return NULL;
    }
    MYSQL_ROW _row = mysql_fetch_row(_res);
    // id [flag] name [flag] passmd5 [flag] question [flag] answer [flag]
    if (_row == NULL)
    {
        mysql_free_result(_res);

        #ifdef DEBUG
            fprintf(stdout, "\t>> 查询失败！\n");
        #endif

        return NULL;
    }

    #ifdef DEBUG
        fprintf(stdout, "\t>> 查询成功！\n");
    #endif

    sprintf(name, "%s", _row[0]);
    mysql_free_result(_res);

    return name;
}

int IsShield(int userid, int tarid)
{
    int ret;
    char sql[256];
    sprintf(sql, "select Shield from `%s`.`%d` where id = %d AND status = %d;", Data_UserInfo, userid, tarid, STA_FRI_NOR);

    #ifdef DEBUG
        fprintf(stdout, "\t[数据库] 查询用户 %d 是否屏蔽 %d\n", userid, tarid);
        fprintf(stdout, "\tSQL::  %s\n", sql);
    #endif

    ret = mysql_real_query(_mysql, sql, strlen(sql));
    if (ret)
        sql_err(__LINE__, _mysql);
    MYSQL_RES * _res = mysql_store_result(_mysql);
    MYSQL_ROW row = mysql_fetch_row(_res);

    if (row == NULL)        // 如果查不到数据，双方不是好友
    {
        #ifdef DEBUG
            COLOR(S_DEFAULT_BACK, fRED);
            fprintf(stdout, "\terror: %d & %d 非好友关系\n", userid, tarid);
            CLOSE();
        #endif

        return 1;
    }

    mysql_free_result(_res);

    #ifdef DEBUG
        fprintf(stdout, "\t>> %s\n", (row[0][0] - '0') ? "屏蔽" : "未被屏蔽");
    #endif

    return (row[0][0] - '0');
}

int LoginAcc(int id, char * pass)
{
    int ret;
    char sql[256];
    sprintf(sql, "select id, passmd5 from `%s`.`user_list` where id = %d AND passmd5 = md5('%s');", Data_ChatBase, id, pass);

    #ifdef DEBUG
        fprintf(stdout, "\t[数据库] 验证用户 %d 登录操作\n", id);
        fprintf(stdout, "\tSQL::  %s\n", sql);
    #endif

    ret = mysql_real_query(_mysql, sql, strlen(sql));
    if (ret)
        sql_err(__LINE__, _mysql);
    MYSQL_RES * _res = mysql_store_result(_mysql);
    MYSQL_ROW row = mysql_fetch_row(_res);

    if (row == NULL)                                // 无数据
    {
        #ifdef DEBUG
            fprintf(stdout, "\t>> 操作结果: 验证失败!\n");
        #endif

        mysql_free_result(_res);
        return 0;                                   
    }

    #ifdef DEBUG
        fprintf(stdout, "\t>> 操作结果: 验证成功!\n");
    #endif

    mysql_free_result(_res);
    return 1;                                       // 结果匹配
}

int RegACC(char * name, char * pass, char * problem, char * answer)
{
    int ret, getid;
    char sql[256];
    MYSQL_RES * _res;
    MYSQL_ROW _row;
    sprintf(sql, "insert into `%s`.`user_list`(username, passmd5, question, answer) values('%s', md5('%s'), '%s', '%s');", Data_ChatBase, name, pass, problem, answer);
    ret = mysql_real_query(_mysql, sql, strlen(sql));

#ifdef DEBUG
    fprintf(stdout, "\t[数据库] 注册 - 修改基础库\n");
    fprintf(stdout, "\tSQL::  %s\n", sql);
#endif

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
    sprintf(sql, "drop table if exists `%s`.`%d`;", Data_UserInfo, getid);
    ret = mysql_real_query(_mysql, sql, strlen(sql));
    if (ret)
        sql_err(__LINE__, _mysql);

    _res = mysql_store_result(_mysql);
    mysql_free_result(_res);
    // 开始为新用户重新创建表
    sprintf(sql, "CREATE TABLE IF NOT EXISTS `%s`.`%d` ( `id`\
    INT UNSIGNED NOT NULL, `name` VARCHAR(45) NOT NULL, `status` INT NOT NULL, `Shield` INT NOT NULL, PRIMARY KEY(`id`, `status`));", Data_UserInfo, getid);
    ret = mysql_real_query(_mysql, sql, strlen(sql));

#ifdef DEBUG
    fprintf(stdout, "\n\t[数据库] 注册 - 为用户创建新表\n");
    fprintf(stdout, "\tSQL::  %s\n", sql);
#endif

    if (ret)
        sql_err(__LINE__, _mysql);
    _res = mysql_store_result(_mysql);
    mysql_free_result(_res);

    // 为新用户创建一个离线消息记录表
    sprintf(sql, "CREATE TABLE IF NOT EXISTS `%s`.`%d` ( `id` INT UNSIGNED NOT NULL, `target` INT NOT NULL, `status` INT NOT NULL, `msg` VARCHAR(512) NOT NULL, `time` DATETIME NOT NULL);", Data_OfflineMsg, getid);
    ret = mysql_real_query(_mysql, sql, strlen(sql));

#ifdef DEBUG
    fprintf(stdout, "\n\t[数据库] 注册 - 为用户创建离线消息记录表\n");
    fprintf(stdout, "\tSQL::  %s\n", sql);
#endif

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
    char sql[256];
    sprintf(sql, "insert into `%s`.`%d` values(%d, '%s', 0, 0);", Data_UserInfo, user_id, friend_id, friend_name);
    ret = mysql_real_query(_mysql, sql, strlen(sql));

#ifdef DEBUG
    fprintf(stdout, "\t[数据库] 添加好友(%d->%d)\n", user_id, friend_id);
    fprintf(stdout, "\tSQL::  %s\n", sql);
#endif

    if (ret)
        sql_err(__LINE__, _mysql);

    MYSQL_RES *_res = mysql_store_result(_mysql);
    mysql_free_result(_res);

    return 1;
}

int ShiFriend(int user_id, int friend_id)
{
    int ret;
    char sql[256];
    sprintf(sql, "update `%s`.`%d` set Shield = 1 where id = %d AND status = 0;", Data_UserInfo, user_id, friend_id);
    ret = mysql_real_query(_mysql, sql, strlen(sql));

#ifdef DEBUG
    fprintf(stdout, "\t[数据库] %d 屏蔽 %d\n", user_id, friend_id);
    fprintf(stdout, "\tSQL::  %s\n", sql);
#endif

    if (ret)
        sql_err(__LINE__, _mysql);
    MYSQL_RES * _res = mysql_store_result(_mysql);
    mysql_free_result(_res);

    return 1;
}

int UnShiFriend(int user_id, int friend_id)
{
    int ret;
    char sql[256];
    sprintf(sql, "update `%s`.`%d` set Shield = 0 where id = %d AND status = 0;", Data_UserInfo, user_id, friend_id);
    ret = mysql_real_query(_mysql, sql, strlen(sql));

#ifdef DEBUG
    fprintf(stdout, "\t[数据库] %d 解除对 %d 屏蔽\n", user_id, friend_id);
    fprintf(stdout, "\tSQL::  %s\n", sql);
#endif

    if (ret)
        sql_err(__LINE__, _mysql);
    MYSQL_RES * _res = mysql_store_result(_mysql);
    mysql_free_result(_res);

    return 1;
}

int _DelFriend(int user_id, int friend_id)
{
    int ret;
    char sql[256];
    sprintf(sql, "delete from `%s`.`%d` where id = %d AND status = 0;", Data_UserInfo, user_id, friend_id);
    ret = mysql_real_query(_mysql, sql, strlen(sql));

#ifdef DEBUG
    fprintf(stdout, "\t[数据库] 删除好友(%d->%d)\n", user_id, friend_id);
    fprintf(stdout, "\tSQL::  %s\n", sql);
#endif

    if (ret)
        sql_err(__LINE__, _mysql);
    MYSQL_RES * _res = mysql_store_result(_mysql);
    mysql_free_result(_res);
    return 1;
}

int DelFriend(int user_id, int friend_id)
{
    int ret0 = _DelFriend(user_id, friend_id);
    int ret1 = _DelFriend(friend_id, user_id);
    return ret0 | ret1;
}

int UserRelaList(int user_id, char reststr[FRI_NUM][50], int flag)
{
    int ret, count = 0, k;
    char sql[256];
    k = flag ? STA_GRP_NOR : STA_FRI_NOR;
    sprintf(sql, "select id, name, Shield from `%s`.`%d` where status = %d order by id;", Data_UserInfo, user_id, k);
    ret = mysql_real_query(_mysql, sql, strlen(sql));

#ifdef DEBUG
    fprintf(stdout, "\t[数据库] 获取用户 %d %s\n", user_id, flag ? "群组列表" : "好友列表");
    fprintf(stdout, "\tSQL::  %s\n", sql);
#endif

    if (ret)
        sql_err(__LINE__, _mysql);
    MYSQL_RES * _res = mysql_store_result(_mysql);
    MYSQL_ROW _row;

    #ifdef DEBUG
        fprintf(stdout, "\t--------------------------------------------------\n");
    #endif
    while ((_row = mysql_fetch_row(_res)))
    {
        sprintf(reststr[count++], "%s%s%s%s%s%s\n", _row[0], _END_,  _row[1], _END_, _row[2], _END_);
        #ifdef DEBUG
            fprintf(stdout, "\t>> id = %s\tname = %s\tStatus = %s\n", _row[0], _row[1], _row[2]);
        #endif
    }
    mysql_free_result(_res);

    return count;
}

int CreateGroup(int owner_id, char * name, char * something)
{
    int ret, groupid;
    char sql[256];
    MYSQL_ROW _row;
    MYSQL_RES * _res;

    // 在ChatR_Base中加入内容
    if (something != NULL)
        sprintf(sql, "insert into `%s`.`%s`(name, create_time, member_num, introduction) values ('%s', CURDATE(), 1, '%s');", Data_ChatBase, Table_Grouplist, name, something);
    else
        sprintf(sql, "insert into `%s`.`%s`(name, create_time, member_num) values ('%s', CURDATE(), 1);", Data_ChatBase, Table_Grouplist, name);

    ret = mysql_real_query(_mysql, sql, strlen(sql));

#ifdef DEBUG
    fprintf(stdout, "\t[数据库] 创建群聊-基础库更改\n");
    fprintf(stdout, "\tSQL::  %s\n", sql);
#endif

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

#ifdef DEBUG
    fprintf(stdout, "\n\t[数据库] 创建群聊-获取新建群聊id\n");
    fprintf(stdout, "\tSQL::  %s\n", sql);
    fprintf(stdout, "\t>> new_group id = %d\n", groupid);
#endif

    // 清除残余表
    sprintf(sql, "drop table if exists `%s`.`%d`;", Data_GroupInfo, groupid);
    ret = mysql_real_query(_mysql, sql, strlen(sql));
    if (ret)
        sql_err(__LINE__, _mysql);
    _res = mysql_store_result(_mysql);
    mysql_free_result(_res);

    // 开始为新群建表
    sprintf(sql, "CREATE TABLE IF NOT EXISTS `%s`.`%d` ( `id` INT UNSIGNED NOT NULL, `status` INT UNSIGNED NOT  NULL, PRIMARY KEY(`id`));", Data_GroupInfo, groupid);
    ret = mysql_real_query(_mysql, sql, strlen(sql));

#ifdef DEBUG
    fprintf(stdout, "\n\t[数据库] 创建群聊-新建群成员信息表\n");
    fprintf(stdout, "\tSQL::  %s\n", sql);
#endif
    if (ret)
        sql_err(__LINE__, _mysql);
    _res = mysql_store_result(_mysql);
    mysql_free_result(_res);
    
    // 开始为新表添加数据
    sprintf(sql, "insert into `%s`.`%d` values(%d, %d);", Data_GroupInfo, groupid, owner_id, GRP_STA_OWN);
    ret = mysql_real_query(_mysql, sql, strlen(sql));

#ifdef DEBUG
    fprintf(stdout, "\n\t[数据库] 创建群聊-更新群成员信息表\n");
    fprintf(stdout, "\tSQL::  %s\n", sql);
#endif

    if (ret)
        sql_err(__LINE__, _mysql);
    _res = mysql_store_result(_mysql);
    mysql_free_result(_res);

    // 更新own_id的关系
    sprintf(sql, "insert into `%s`.`%d` values(%d, '%s', %d, %d);",  Data_UserInfo, owner_id, groupid, name, STA_GRP_NOR, STA_BE_NORM);

#ifdef DEBUG
    fprintf(stdout, "\n\t[数据库] 创建群聊-更新群主群组列表\n");
    fprintf(stdout, "\tSQL::  %s\n", sql);
#endif

    ret = mysql_real_query(_mysql, sql, strlen(sql));
    if (ret)
        sql_err(__LINE__, _mysql);
    _res = mysql_store_result(_mysql);
    mysql_free_result(_res);

    return groupid;
}

char* SearchGrpId(int id, char * grp)
{
    int ret;
    char sql[256];
    MYSQL_RES * _res;
    MYSQL_ROW  _row;
    sprintf(sql, "select name from `%s`.`%s` where id = %d;", Data_ChatBase, Table_Grouplist, id);
    ret = mysql_real_query(_mysql, sql, strlen(sql));

#ifdef DEBUG
    fprintf(stdout, "\t[数据库] 查找群 %d\n", id);
    fprintf(stdout, "\tSQL::  %s\n", sql);
#endif

    if (ret)
        sql_err(__LINE__, _mysql);
    _res = mysql_store_result(_mysql);

    _row = mysql_fetch_row(_res);
    if (_row == NULL)
    {
        #ifdef DEBUG
            fprintf(stdout, "\t>> 操作结果: 查询失败!\n");
        #endif

        mysql_free_result(_res);
        return NULL;
    }
    // id [flag] name [flag] create_time [flag] member_num [flag] introduction [flag]
    if (grp)
        sprintf(grp, "%s", _row[0]);

    #ifdef DEBUG
        fprintf(stdout, "\t>> 操作结果: 查询成功!\n");
    #endif

    mysql_free_result(_res);
    return grp;
}

int AddOneToGrp(int user_id, char * user_name, int grp_id, char * grp_name)
{
    int ret;
    char sql[256];
    MYSQL_RES * _res;
    sprintf(sql, "insert into `%s`.`%d` values(%d, %d);", Data_GroupInfo, grp_id, user_id, GRP_STA_NOR);
    ret = mysql_real_query(_mysql, sql, strlen(sql));

#ifdef DEBUG
    fprintf(stdout, "\t[数据库] 添加群成员-群成员信息更改\n");
    fprintf(stdout, "\tSQL::  %s\n", sql);
#endif

    if (ret)
        sql_err(__LINE__, _mysql);
    _res = mysql_store_result(_mysql);
    mysql_free_result(_res);
    
    sprintf(sql, "insert into `%s`.`%d` values(%d, '%s', %d, %d);", Data_UserInfo, user_id, grp_id, grp_name, STA_GRP_NOR, STA_BE_NORM);
    ret = mysql_real_query(_mysql, sql, strlen(sql));

#ifdef DEBUG
    fprintf(stdout, "\n\t[数据库] 添加群成员-更新群成员群关系\n");
    fprintf(stdout, "\tSQL::  %s\n", sql);
#endif

    if (ret)
        sql_err(__LINE__, _mysql);
    _res = mysql_store_result(_mysql);
    mysql_free_result(_res);
    
    return 1;
}

int GrpMemberList(int grp_id, int mem_id[MEM_NUM], int mem_sta[MEM_NUM])
{
    int ret, count = 0;
    char sql[256];
    MYSQL_RES * _res;
    MYSQL_ROW _row;
    sprintf(sql, "select id, status from `%s`.`%d`;", Data_GroupInfo, grp_id);
    ret = mysql_real_query(_mysql, sql, strlen(sql));
    
#ifdef DEBUG
    fprintf(stdout, "\t[数据库] 获取群 %d 成员列表\n", grp_id);
    fprintf(stdout, "\tSQL::  %s\n", sql);
#endif

    if (ret)    
        sql_err(__LINE__, _mysql);
    _res = mysql_store_result(_mysql);

    #ifdef DEBUG
        fprintf(stdout, "--------------------------------------------------\n");
    #endif

    while ((_row = mysql_fetch_row(_res)))
    {
        if (mem_id)
            mem_id[count] = atoi(_row[0]);
        if (mem_sta)
            mem_sta[count] = atoi(_row[1]);

        #ifdef DEBUG
            if (mem_id)
                fprintf(stdout, "\t>> userid = %d\t", mem_id[count]);
            if (mem_sta)
                fprintf(stdout, "status = %d\n", mem_sta[count]);
        #endif

        count++;
    }

    mysql_free_result(_res);

    return count;
}

int OfflineMSG(Package * msg, int tar_id, int flag)
{
    int ret;
    char sql[1024], temp[512];
    strcpy(temp, msg->strmsg);

    sprintf(sql, "insert into `%s`.`%d` values( %d, %d, %d, '%s', now());", Data_OfflineMsg, tar_id, msg->source_id, msg->target_id, flag, TranToEsc(temp));

#ifdef DEBUG
    fprintf(stdout, "\t[数据库] 存储用户 %d 离线消息\n", tar_id);
    fprintf(stdout, "\tSQL::  %s\n", sql);
#endif

    ret = mysql_real_query(_mysql, sql, strlen(sql));
    if (ret)
    {
        fprintf(stderr, "%s\n", sql);
        sql_err(__LINE__, _mysql);
    }

    MYSQL_RES * _res = mysql_store_result(_mysql);
    mysql_free_result(_res);
    return 1;
}

// 在群里查找 id == needs_id 的用户
int SearchOneInGrp(int grp_id, int needs_id)
{
    int ret;
    char sql[256];
    sprintf(sql, "select id from `%s`.`%d` where id = %d;", Data_GroupInfo, grp_id, needs_id);
    ret = mysql_real_query(_mysql, sql, strlen(sql));

#ifdef DEBUG
    fprintf(stdout, "\t[数据库] 在群 %d 查询用户 %d\n", grp_id, needs_id);
    fprintf(stdout, "\tSQL::  %s\n", sql);
#endif

    if (ret)
        sql_err(__LINE__, _mysql);
    MYSQL_RES * _res = mysql_store_result(_mysql);
    MYSQL_ROW _row = mysql_fetch_row(_res);

    ret = (!_row) ? 0 : 1;

    mysql_free_result(_res);

#ifdef DEBUG
    if (ret)
        fprintf(stdout, "\t>> 查询成功!\n");
    else
        fprintf(stdout, "\t>> 查询失败!\n");
#endif

    return ret;
}

// 在群里查询 身份为　status 的用户，　并将id存入box数组中
int* SearchStaGrp(int grp_id, int status, int box[MEM_NUM])
{
    int ret, count = 0;
    char sql[256];
    sprintf(sql, "select id from `%s`.`%d` where status = %d;", Data_GroupInfo, grp_id, status);
    ret = mysql_real_query(_mysql, sql, strlen(sql));

#ifdef DEBUG
    fprintf(stdout, "\t[数据库] 在群 %d 查找状态为 %d 的用户\n", grp_id, status);
    fprintf(stdout, "\tSQL::  %s\n", sql);
#endif

    if (ret)
        sql_err(__LINE__, _mysql);
    MYSQL_RES * _res = mysql_store_result(_mysql);
    MYSQL_ROW _row;

    while ((_row = mysql_fetch_row(_res)))
    {
        box[count++] = atoi(_row[0]);

        #ifdef DEBUG
            fprintf(stdout, "\t>> userid = %d\n", box[count - 1]);
        #endif
    }
    mysql_free_result(_res);

    return count ? box : NULL;
}

// 给用户传递离线消息 
int TransOffMsg(int usr_id, char message[OffMsg_NUM][256])
{
    int ret, count = 0;
    char sql[256];
    sprintf(sql, "select id, target, status, msg, time from `%s`.`%d` order by status asc, id asc, time asc;", Data_OfflineMsg, usr_id);
    ret = mysql_real_query(_mysql, sql,strlen(sql));

#ifdef DEBUG
    fprintf(stdout, "\t[数据库] 传递用户 %d 离线消息\n", usr_id);
    fprintf(stdout, "\tSQL::  %s\n", sql);
#endif

    if (ret)
        sql_err(__LINE__, _mysql);
    MYSQL_RES * _res = mysql_store_result(_mysql);
    MYSQL_ROW _row;
    
    while ((_row = mysql_fetch_row(_res)))
    {
        sprintf(message[count], "%s%s%s%s%s%s%s%s%s%s", _row[0], _END_, _row[1], _END_, _row[2], _END_, _row[3], _END_, _row[4], _END_);
        count++;
    }
    mysql_free_result(_res);

    sprintf(sql, "delete from `%s`.`%d`;", Data_OfflineMsg, usr_id);

#ifdef DEBUG    
    fprintf(stdout, "\n\t[数据库] 清除临时数据\n");
    fprintf(stdout, "\tSQL::  %s\n", sql);
#endif

    ret = mysql_real_query(_mysql, sql,strlen(sql));
    if (ret)
        sql_err(__LINE__, _mysql);
    _res = mysql_store_result(_mysql);
    mysql_free_result(_res);

    return count;
}

// 从群组中移除一个人
int ReOneFromGrp(int user_id, int grp_id)
{
    int ret;
    char sql[512];
    MYSQL_RES * _res;
    sprintf(sql, "delete from `%s`.`%d` where id = %d AND status = %d;", Data_UserInfo, user_id, grp_id, STA_GRP_NOR);

#ifdef DEBUG
    fprintf(stdout, "\t[数据库] 更新用户 %d 关系表群组信息\n", user_id);
    fprintf(stdout, "\tSQL::  %s\n", sql);
#endif

    ret = mysql_real_query(_mysql, sql, strlen(sql));
    if (ret)
    {
        fprintf(stderr, "%s\n", sql);
        sql_err(__LINE__, _mysql);
    }
    _res = mysql_store_result(_mysql);
    mysql_free_result(_res);

    sprintf(sql, "delete from `%s`.`%d` where id = %d;", Data_GroupInfo, grp_id, user_id);

#ifdef DEBUG
    fprintf(stdout, "\n\t[数据库] 更新群 %d 成员相关信息\n", grp_id);
    fprintf(stdout, "\tSQL::  %s\n", sql);
#endif

    ret = mysql_real_query(_mysql, sql, strlen(sql));
    if (ret)
    {
        fprintf(stderr, "%s\n", sql);
        sql_err(__LINE__, _mysql);
    }
    _res = mysql_store_result(_mysql);
    mysql_free_result(_res);

    return 1;
}

// 删除群
void DelGroup(int grp_id)
{
    int ret, count;
    char sql[512];
    int memid[MEM_NUM];
    MYSQL_RES * _res;
    MYSQL_ROW _row;

    sprintf(sql, "delete from `%s`.`%s` where id = %d;", Data_ChatBase, Table_Grouplist, grp_id);

#ifdef DEBUG
    fprintf(stdout, "\t[数据库] 删除群 - 更新基础信息表\n");
    fprintf(stdout, "\tSQL::  %s\n", sql);
#endif

    ret = mysql_real_query(_mysql, sql, strlen(sql));
    if (ret)
    {
        fprintf(stderr, "%s\n", sql);
        sql_err(__LINE__, _mysql);
    }
    _res = mysql_store_result(_mysql);
    mysql_free_result(_res); 

    sprintf(sql, "select id from `%s`.`%d`;", Data_GroupInfo, grp_id);

#ifdef DEBUG
    fprintf(stdout, "\n\t[数据库] 删除群 - 移除群成员\n");
    fprintf(stdout, "\tSQL::  %s\n\n", sql);
#endif

    ret = mysql_real_query(_mysql, sql, strlen(sql));
    if (ret)
    {
        fprintf(stderr, "%s\n", sql);
        sql_err(__LINE__, _mysql);
    }
    _res = mysql_store_result(_mysql);
    count = 0;
    while ((_row = mysql_fetch_row(_res)))
    {
        memid[count] = atoi(_row[0]);
        count++;      
    }
    mysql_free_result(_res); 
    for (int i = 0; i < count; i++)
    {
        ReOneFromGrp(memid[i], grp_id);
    }

    // 最后删群成员列表，需要查看群成员列表来删除对应用户的关系表
    sprintf(sql, "drop table `%s`.`%d`;", Data_GroupInfo, grp_id);

#ifdef DEBUG
    fprintf(stdout, "\n\t[数据库] 删除群 - 删除群\n");
    fprintf(stdout, "\tSQL::  %s\n", sql);
#endif

    ret = mysql_real_query(_mysql, sql, strlen(sql));
    if (ret)
    {
        fprintf(stderr, "%s\n", sql);
        sql_err(__LINE__, _mysql);
    }
    _res = mysql_store_result(_mysql);
    mysql_free_result(_res); 
}

// 设置管理员
void SetCtrl(int grp_id, int user_id)
{
    int ret;
    char sql[512];
    MYSQL_RES * _res;
    sprintf(sql, "update `%s`.`%d` set status = %d where id = %d", Data_GroupInfo, grp_id, GRP_STA_CON, user_id);

#ifdef DEBUG
    fprintf(stdout, "\t[数据库] 设置 group = %d userid = %d 为管理员\n", grp_id, user_id);
    fprintf(stdout, "\tSQL::  %s\n", sql);
#endif

    ret = mysql_real_query(_mysql, sql, strlen(sql));
    if (ret)
        sql_err(__LINE__, _mysql);
    _res = mysql_store_result(_mysql);
    mysql_free_result(_res);
}

// 取消管理员设置
void UnSetCtrl(int grp_id, int user_id)
{
    int ret;
    char sql[512];
    MYSQL_RES * _res;
    sprintf(sql, "update `%s`.`%d` set status = %d where id = %d", Data_GroupInfo, grp_id, GRP_STA_NOR, user_id);

#ifdef DEBUG
    fprintf(stdout, "\t[数据库] 取消 group = %d userid = %d 管理员权限\n", grp_id, user_id);
    fprintf(stdout, "\tSQL::  %s\n", sql);
#endif

    ret = mysql_real_query(_mysql, sql, strlen(sql));
    if (ret)
        sql_err(__LINE__, _mysql);
    _res = mysql_store_result(_mysql);
    mysql_free_result(_res);
}
