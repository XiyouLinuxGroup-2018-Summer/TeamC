#include "sql.h"
#define DEBUG

extern MYSQL * _mysql;

void* sql_err(int line, MYSQL * _mysql)
{
    fprintf(stderr, "%d: Error: %s\n", line, mysql_error(_mysql));
    return NULL;
}

char* SearchAccId(int id, char * name)
{
    int ret;
    char sql[256];
    sprintf(sql, "select username from `%s`.`%s` where id = %d;", Data_ChatBase, Table_Userlist, id);
#ifdef DEBUG
    printf("通过用户id查找用户名字\n");
    printf("sql: %s\n", sql);
#endif
    ret = mysql_real_query(_mysql, sql, strlen(sql));
    if (ret)
        sql_err(__LINE__, _mysql);
    MYSQL_RES * _res = mysql_store_result(_mysql);
    if (_res == NULL)
    {
        if (mysql_field_count(_mysql) == 0)
            return NULL;
        else
            sql_err(__LINE__, _mysql);
    }
    MYSQL_ROW _row = mysql_fetch_row(_res);
    // id [flag] name [flag] passmd5 [flag] question [flag] answer [flag]
    sprintf(name, "%s", _row[0]);

    mysql_free_result(_res);
    //?????malloc??
    return name;
}

int LoginAcc(int id, char * pass)
{
    int ret;
    char sql[256];
    sprintf(sql, "select id, passmd5 from `%s`.`user_list` where id = %d AND passmd5 = md5('%s');", Data_ChatBase, id, pass);
#ifdef DEBUG
    printf("登录操作\n");
    printf("sql: %s\n", sql);
#endif
    ret = mysql_real_query(_mysql, sql, strlen(sql));
    if (ret)
        sql_err(__LINE__, _mysql);
    MYSQL_RES * _res = mysql_store_result(_mysql);
    MYSQL_ROW row = mysql_fetch_row(_res);

    if (row == NULL)                                // 无数据
    {
        mysql_free_result(_res);
        return 0;                                   // 结果匹配
    }
    mysql_free_result(_res);
    return 1;                                   // 结果匹配
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
    printf("注册操作 - 修改基础库\n");
    printf("sql: %s\n", sql);
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
    printf("注册操作 - 为用户创建新表\n");
    printf("sql: %s\n", sql);
#endif
    if (ret)
        sql_err(__LINE__, _mysql);
    _res = mysql_store_result(_mysql);
    mysql_free_result(_res);

    // 为新用户创建一个离线消息记录表
    sprintf(sql, "CREATE TABLE IF NOT EXISTS `%s`.`%d` ( `id` INT UNSIGNED NOT NULL, `target` INT NOT NULL, `status` INT NOT NULL, `msg` VARCHAR(512) NOT NULL, `time` DATETIME NOT NULL);", Data_OfflineMsg, getid);
    ret = mysql_real_query(_mysql, sql, strlen(sql));
#ifdef DEBUG
    printf("注册操作 - 为用户创建离线消息记录表\n");
    printf("sql: %s\n", sql);
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
    printf("添加好友-单\n");
    printf("sql: %s\n", sql);
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
    printf("屏蔽好友\n");
    printf("sql: %s\n", sql);
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
    printf("解除屏蔽好友\n");
    printf("sql: %s\n", sql);
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
    printf("删除好友-单\n");
    printf("sql: %s\n", sql);
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
    printf("temp at %p\n", reststr);
    sprintf(sql, "select id, name, Shield from `%s`.`%d` where status = %d order by id;", Data_UserInfo, user_id, k);
    ret = mysql_real_query(_mysql, sql, strlen(sql));
#ifdef DEBUG
    printf("获取用户好友列表或群组列表\n");
    printf("sql: %s\n", sql);
#endif
    if (ret)
        sql_err(__LINE__, _mysql);
    MYSQL_RES * _res = mysql_store_result(_mysql);
    MYSQL_ROW _row;

    while ((_row = mysql_fetch_row(_res)))
    {
        sprintf(reststr[count++], "%s%s%s%s%s%s\n", _row[0], _END_,  _row[1], _END_, _row[2], _END_);
        printf("reststr[count] = %s\n", reststr[count-1]);
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
    printf("创建群聊-基础库更改\n");
    printf("sql: %s\n", sql);
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
    printf("创建群聊-获取新建群聊id\n");
    printf("sql: %s\n", sql);
    printf("id = %d\n", groupid);
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
    printf("创建群聊-新建群成员信息表\n");
    printf("sql: %s\n", sql);
#endif
    if (ret)
        sql_err(__LINE__, _mysql);
    _res = mysql_store_result(_mysql);
    mysql_free_result(_res);
    
    // 开始为新表添加数据
    sprintf(sql, "insert into `%s`.`%d` values(%d, %d);", Data_GroupInfo, groupid, owner_id, GRP_STA_OWN);
    ret = mysql_real_query(_mysql, sql, strlen(sql));
#ifdef DEBUG
    printf("创建群聊-向群成员信息表中添加数据\n");
    printf("sql: %s\n", sql);
#endif
    if (ret)
        sql_err(__LINE__, _mysql);
    _res = mysql_store_result(_mysql);
    mysql_free_result(_res);

    // 更新own_id的关系
    sprintf(sql, "insert into `%s`.`%d` values(%d, '%s', %d, %d);",  Data_UserInfo, owner_id, groupid, name, STA_GRP_NOR, STA_BE_NORM);
#ifdef DEBUG
    printf("创建群聊-更新群主的群组列表\n");
    printf("sql: %s\n", sql);
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
    printf("查找群名\n");
    printf("sql: %s\n", sql);
#endif
    if (ret)
        sql_err(__LINE__, _mysql);
    _res = mysql_store_result(_mysql);

    _row = mysql_fetch_row(_res);
    if (_row == NULL)
    {
        mysql_free_result(_res);
        return NULL;
    }
    // id [flag] name [flag] create_time [flag] member_num [flag] introduction [flag]
    sprintf(grp, "%s", _row[0]);

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
    printf("添加群成员-群成员信息更改\n");
    printf("sql: %s\n", sql);
#endif
    if (ret)
        sql_err(__LINE__, _mysql);
    _res = mysql_store_result(_mysql);
    mysql_free_result(_res);
    
    sprintf(sql, "insert into `%s`.`%d` values(%d, '%s', %d, %d);", Data_UserInfo, user_id, grp_id, grp_name, STA_GRP_NOR, STA_BE_NORM);
    ret = mysql_real_query(_mysql, sql, strlen(sql));
#ifdef DEBUG
    printf("添加群成员-群成员群关系更新\n");
    printf("sql: %s\n", sql);
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
    printf("获取群成员列表\n");
    printf("sql: %s\n", sql);
#endif
    if (ret)    
        sql_err(__LINE__, _mysql);
    _res = mysql_store_result(_mysql);

    while ((_row = mysql_fetch_row(_res)))
    {
        if (mem_id)
            mem_id[count] = atoi(_row[0]);
        if (mem_sta)
            mem_sta[count] = atoi(_row[1]);
        if (mem_id)
            printf("memid = %d\n", mem_id[count]);
        if (mem_sta)
            printf("memstd = %d\n", mem_sta[count]);
        count++;
    }
    mysql_free_result(_res);

    return count;
}

int OfflineMSG(Package * msg, int tar_id, int flag)
{
    int ret;
    char sql[1024];
    sprintf(sql, "insert into `%s`.`%d` values( %d, %d, %d, '%s', now());", Data_OfflineMsg, tar_id, msg->source_id, msg->target_id, flag, msg->strmsg);
    ret = mysql_real_query(_mysql, sql, strlen(sql));
#ifdef DEBUG
    printf("离线消息存入\n");
    printf("sql: %s\n", sql);
#endif
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
    printf("在群里查找id== needs_id 的用户\n");
    printf("sql: %s\n", sql);
#endif
    if (ret)
        sql_err(__LINE__, _mysql);
    MYSQL_RES * _res = mysql_store_result(_mysql);
    MYSQL_ROW _row = mysql_fetch_row(_res);

    ret = (_row == NULL) ? 0 : 1;

    mysql_free_result(_res);

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
    printf("在群里查找status 的用户\n");
    printf("sql: %s\n", sql);
#endif
    if (ret)
        sql_err(__LINE__, _mysql);
    MYSQL_RES * _res = mysql_store_result(_mysql);
    MYSQL_ROW _row;

    while ((_row = mysql_fetch_row(_res)))
    {
        box[count++] = atoi(_row[0]);
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
    printf("传递离线消息\n");
    printf("sql: %s\n", sql);
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
    printf("清除数据库中的临时数据\n");
    printf("sql: %s\n", sql);
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
    printf("移除用户关系表中的群组信息\n");
    printf("sql: %s\n", sql);
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
    printf("移除群组成员相关信息\n");
    printf("sql: %s\n", sql);
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
    printf("删除群 - 删除基础表中的群记录\n");
    printf("sql: %s\n", sql);
    #endif
    ret = mysql_real_query(_mysql, sql, strlen(sql));
    if (ret)
    {
        fprintf(stderr, "%s\n", sql);
        sql_err(__LINE__, _mysql);
    }
    _res = mysql_store_result(_mysql);
    mysql_free_result(_res); 

    sprintf(sql, "select id from `%s`.`%d`;", Data_UserInfo, grp_id);
    #ifdef DEBUG
    printf("删除群 - 删除群里的每个用户\n");
    printf("sql: %s\n", sql);
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
    sprintf(sql, "drop table `%s`.`%d`;", Data_UserInfo, grp_id);
    #ifdef DEBUG
    printf("删除群 - 删除群组成员列表\n");
    printf("sql: %s\n", sql);
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
        printf("设置管理员操作");
        printf("sql: %s\n", sql);
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
        printf("设置管理员操作");
        printf("sql: %s\n", sql);
    #endif
    ret = mysql_real_query(_mysql, sql, strlen(sql));
    if (ret)
        sql_err(__LINE__, _mysql);
    _res = mysql_store_result(_mysql);
    mysql_free_result(_res);
}
