drop database if exists chatting-room;
create database chatting-room;
use chatting-room;

drop table if exists users;

 create table users(`users_id` INT UNSIGNED AUTO_INCREMENT,`users_number` VARCHAR(100),`users_passwd` VARCHAR(100),`users_name` VARCHAR(100),`users_qusetion` VARCHAR(100),`users_answer` VARCHAR(100),PRIMARY KEY ( `users_id` ))ENGINE=InnoDB DEFAULT CHARSET=utf8;



drop table if exists chat_record ;   /*个人聊天记录*/
create table chat_record ( come INT UNSIGNED  ,go   INT UNSIGNED ,record VARCHAR(40) , time  TIMESTAMP DEFAULT CURRENT_TIMESTAMP) ;

drop table if exists qun_chat_record ; /*群聊天记录*/
CREATE TABLE `msg_config` ( `id` bigint(20) NOT NULL AUTO_INCREMENT,`gid` bigint(20) DEFAULT NULL,  `last_msg_id` bigint(20) DEFAULT NULL,  PRIMARY KEY (`id`) ) ENGINE=InnoDB DEFAULT CHARSET=utf8;


drop table if exists off_line_record ; /*离线消息，只存储个人聊天的消息和群聊消息*/
create table off_line_record( go_QQ INT UNSIGNED  , come_QQ  INT UNSIGNED , record VARCHAR(40), mode     INT  ) ;


drop table if exists system_log ;    /*系统日志*/
create table system_log( user_QQ  INT UNSIGNED  , username VARCHAR(32), record VARCHAR(40) , time  TIMESTAMP DEFAULT CURRENT_TIMESTAMP) ;


