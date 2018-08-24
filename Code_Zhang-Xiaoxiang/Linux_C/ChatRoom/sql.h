/* 
    -------- 与mysql有关的操作 --------
    [--- 对账号的操作都要基于用户id ---]
    > ------------ 查询账号 ------------
        |------ id/用户名 -------|
    > ------------ 注册账号 ------------
    > ------------ 登录账号 ------------
        -------------修改账号 ------------
    > ------------ 添加好友 ------------
    > ------------ 屏蔽好友 ------------
    > ------------ 用户关系列表 ------------
    // 单条发送，以NULL结尾
    > ------------ 删除好友 ------------
    > ------------ 新建群组 ------------
    > ------------ 查找群组 ------------
    > ------------ 加入群组 ------------
    ------------ 移出群组 ------------
    ------------ 解散群组 ------------
    > ------------ 离线消息 ------------
    > ------------ 群成员列表 -----------
    [================================]
 */
#ifndef __ACCOUNT_H_
#define __ACCOUNT_H_
#include "base.h"
#include "internet.h"

// 用到的数据库信息
#define MS_USER             "root"
#define MS_PASS             "mysqlpass"
#define Data_ChatBase       "ChatR_Base"
#define Data_OfflineMsg     "offlineMSG"         
#define Table_Userlist      "user_list"
#define Table_Grouplist     "group_list"      
#define Data_UserInfo       "user_info"
#define Data_GroupInfo      "group_info"

// Data_UserInfo 状态信息
#define STA_FRI_NOR         0       // 好友正常状态
#define STA_GRP_NOR         1       // 群组正常状态
#define STA_BE_NORM         0       // 未被屏蔽
#define STA_BE_SHID         1       // 被屏蔽
#define GRP_STA_OWN         2       // 群主
#define GRP_STA_CON         1       // 管理员
#define GRP_STA_NOR         0       // 普通群员

/* ----------------------------------- function ----------------------------------- */

// fun: mysql错误处理
void* sql_err(int line, MYSQL * _mysql);
// fun: 根据用户id查询用户
// arg->id: 用户id
// arg->acc: 存储查找后用户的信息
// return : 查找成功返回acc, 失败返回NULL
// note: 如果成功需要free(acc)
char* SearchAccId(int id, char * acc);

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

int UnShiFriend(int user_id, int friend_id);

// DelFriend() 的子函数，只操作一方
int _DelFriend(int user_id, int friend_id);

// fun: 删除好友
// arg->userid: 执行删除操作的用户
// arg->friend_id: 被删的用户
// return: 成功返回1，失败返回0
int DelFriend(int user_id, int friend_id);

// fun: 用户关系列表 
// arg->user_id: 用户id
// arg->reststr: 用于存储查询到的用户的好友列表, 二维数组, 用 calloc 分配，
//               _END_ 分割参数，以NULL结尾
// arg->flag: flag = 0, 好友列表，flag = 1, 群组列表
// return: 成功返回指向 reststr 的指针，发生错误返回NUll
int UserRelaList(int user_id, char reststr[FRI_NUM][50], int flag);

// fun: 创建群
// note: 更新ChatR_Base.group_list, 为群组新建一个表， 为新建的表添加数据，更新群主(用户)的关系列表
// 群主id, 群名， 群介绍， 群成员人数， 群mysql_insert_id
// 返回群id
// something 为 NULL, 使用默认介绍
int CreateGroup(int owner_id, char * name, char * something);

// fun: 大体同SearchAccId
char* SearchGrpId(int id, char * grp);

// fun: 加入群组
// note: 修改群组成员信息，修改个人关系列表
int AddOneToGrp(int user_id, char * user_name, int grp_id, char * grp_name);

// fun: 群成员列表
int GrpMemberList(int grp_id, int mem_id[MEM_NUM], char mem_name[MEM_NUM][USER_NAME_MAX + 1], int mem_sta[MEM_NUM]);

// fun: 离线消息
int OfflineMSG(Package * msg, int tar_id, int flag);

// 在群里查找 id == needs_id 的用户
int SearchOneInGrp(int grp_id, int needs_id);

// 在群里查询 身份为　status 的用户，　并将id存入box数组中
int* SearchStaGrp(int grp_id, int status, int box[MEM_NUM]);

// 给用户传递离线消息 
int TransOffMsg(int usr_id, char message[OffMsg_NUM][256]);

#endif
