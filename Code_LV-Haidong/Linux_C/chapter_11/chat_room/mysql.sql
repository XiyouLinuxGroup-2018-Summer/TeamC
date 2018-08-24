 create table users(`users_id` INT UNSIGNED AUTO_INCREMENT,`users_number`VARCHAR(100),`users_passwd` VARCHAR(100),PRIMARY KEY ( `runoob_id` ))ENGINE=InnoDB DEFAULT CHARSET=utf8;


create table firends(`firends_id` INT UNSIGNED AUTO_INCREMENT,`firends_number`VARCHAR(100),`firends_name` VARCHAR(100),`firends_sex` VARCHAR(100),PRIMARY KEY ( `users_id` ))ENGINE=InnoDB DEFAULT CHARSET=utf8;

CREATE TABLE `msg` (   `id` bigint(20) NOT NULL AUTO_INCREMENT,   `gid` bigint(20) DEFAULT NULL,   `content` text DEFAULT NULL,   `create_time` datetime DEFAULT NULL,   `update_time` datetime DEFAULT NULL,   PRIMARY KEY (`id`) ) ENGINE=InnoDB DEFAULT CHARSET=utf8;

CREATE TABLE `msg_config` (   `id` bigint(20) NOT NULL AUTO_INCREMENT,   `gid` bigint(20) DEFAULT NULL,   `last_msg_id` bigint(20) DEFAULT NULL,   PRIMARY KEY (`id`) ) ENGINE=InnoDB DEFAULT CHARSET=utf8;

create table users(`users_id` INT UNSIGNED AUTO_INCREMENT,`users_number`VARCHAR(100),`users_passwd` VARCHAR(100),`users_name` VARCHR(100),`users_qusetion` VARCHAR(100),`users_answer` VARCHR(100),PRIMARY KEY ( `users_id` ))ENGINE=InnoDB DEFAULT CHARSET=utf8;


alter table users add unique key(`users_number`);
//设置unique key 建防止重复
