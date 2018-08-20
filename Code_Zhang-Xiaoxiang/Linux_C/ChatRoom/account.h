/* 
    -------- 与账号有关的操作 --------
    [--- 对账号的操作都要基于用户id ---]
    > ------------ 查询账号 ------------
        |------ id/用户名 -------|
    > ------------ 注册账号 ------------
    > ------------ 登录账号 ------------
        -------------修改账号 ------------
    > ------------ 添加好友 ------------
    > ------------ 屏蔽好友 ------------
    > ------------ 好友列表 ------------
    // 单条发送，以NULL结尾
    > ------------ 删除好友 ------------
    > ------------ 新建群组 ------------
    ------------ 查找群组 ------------
    ------------ 加入群组 ------------
    ------------ 移出群组 ------------
    ------------ 解散群组 ------------
    ------------ 群组列表 ------------
    [================================]
 */
#ifndef __ACCOUNT_H_
#define __ACCOUNT_H_
#include "base.h"

// 用到的数据库信息
#define MS_USER             "root"
#define MS_PASS             "mysqlpass"
#define Data_ChatBase       "ChatR_Base"
#define Table_Userlist      "user_list"
#define Table_Grouplist     "group_list"      
#define Data_UserInfo       "user_info"
#define Data_GroupInfo      "group_info"
// 为新用户创建列表模板
#define USER_INFO_CRETABLE(database, x) "CREATE TABLE IF NOT EXISTS `"#database"`.`"#x"` ( `id`\
    INT UNSIGNED NOT NULL, `name` VARCHAR(45) NOT NULL, `status` INT NOT NULL, `Shield` INT NOT NULL, PRIMARY KEY(`id`, `status`));"
// 删除某张表
#define DEL_TABLE(database, x) "drop table if exists `"#database"`.`"#x"`;"

// Data_UserInfo 状态信息
#define STA_FRI_NOR         0       // 好友正常状态
#define STA_GRP_NOR         1       // 群组正常状态
#define STA_BE_NORM         0       // 未被屏蔽
#define STA_BE_SHID         1       // 被屏蔽
#define GRP_STA_OWN         2       // 群主
#define GRP_STA_CON         1       // 管理员
#define GRP_STA_NOR         0       // 普通群员

// 数据库操作句柄等全局变量
static MYSQL * _mysql = NULL;
static MYSQL_ROW _row;
static MYSQL_RES * _res;

// 相关结构体定义
typedef struct account      // the struct for user
{
    int id;                 // user id
    char name[32];          // user name
    char passmd5[35];       // user passmd5
    char question[100];     // question for retrieve password
    char answer[100];       // answer
}Account;

typedef struct group                // the struct for group
{
    int id;                         // group id
    int num_of_member;              // members number 
    char name[32];                  // group name
    char create_time[40];           // the group create time
    char introduction[1024];        // the introduction from introduction
}Group;

/* ----------------------------------- function ----------------------------------- */

// fun: 根据用户id查询用户
// arg->id: 用户id
// arg->acc: 存储查找后用户的信息
// return : 查找成功返回acc, 失败返回NULL
// note: 如果成功需要free(acc)
Account* SearchAccId(int id, Account * acc);

// fun: 根据用户名查找服务器上的用户信息
// arg->name: 用户名
// arg->res_str: 存储查找结果的字符串，查找成功存入查找的信息，失败或无数据则标示为 \r\t\n
void SearchAccNm(char * name, char * res_str);

// fun: 登录账号
// arg->name: 用户id
// arg->pass: 输入的密码
// return: 匹配成功返回1，匹配失败返回0
int LoginAcc(int id, char * pass);

// fun: 注册账号
// arg->name: 用户id
// arg->pass: 用户输入的密码
// arg->problem: 密保问题(可为空NULL)
// arg->answer: 密保回答
// return: 失败返回0，成功返回新增用户的id
// note: 注册后需要为新用户创建一张好友列表、群组列表，不会检查为用户单独建表过程中出现的问题
int RegACC(char * name, char * pass, char * problem , char * answer);

// fun: 添加好友
// arg->user_id: 操作者id
// arg->friend_id: 要添加的好友
// return: 成功返回1，失败返回0
// note: 必须保证要添加的好友不在好友列表中，两方都要添加
int AddFriend(int user_id, int friend_id, char * user_name, char * friend_name);

// AddFriend() 的子函数，只操作一方的数据
int _AddFriend(int user_id, int friend_id, char * friend_name);

// fun: 屏蔽好友
// arg->user_id: 操作的用户
// arg->friend_id: 用户要屏蔽的好友
// return: 成功返回1，失败返回0
int ShiFriend(int user_id, int friend_id);

// DelFriend() 的子函数，只操作一方
int _DelFriend(int user_id, int friend_id);

// fun: 删除好友
// arg->userid: 执行删除操作的用户
// arg->friend_id: 被删的用户
// return: 成功返回1，失败返回0
int DelFriend(int user_id, int friend_id);

// fun: 好友列表
// arg->user_id: 用户id
// arg->reststr: 用于存储查询到的用户的好友列表
// return: 成功返回指向 reststr 的指针，发生错误返回0
// note: 当用户登录时候给用户传输此信息，之后的变化由客户端申请用户关系变化，申请成功后服务器处理相关信息，然后在本地修改用户关系表
char* FriendList(int user_id, char * reststr);

// fun: 创建群
// note: 更新ChatR_Base.group_list, 为群组新建一个表， 为新建的表添加数据，更新群主(用户)的关系列表
int CreateGroup(int owner_id, char * name, char * createtime, char * something);

#endif
