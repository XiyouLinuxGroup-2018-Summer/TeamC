/* 创建用户与群组基本数据库 */
CREATE DATABASE IF NOT EXISTS ChatR_Base;

/* 创建user_list表 */
CREATE TABLE IF NOT EXISTS `ChatR_Base`.`user_list` (
  `id` INT  UNSIGNED NOT NULL AUTO_INCREMENT,
  `username` VARCHAR(45)  CHARACTER SET 'utf8' NOT NULL,
  `passmd5` VARCHAR(45)  NOT NULL,
  `question` VARCHAR(45) DEFAULT NULL,
  `answer` VARCHAR(45) DEFAULT NULL,
  PRIMARY KEY (`id`))
ENGINE = InnoDB
DEFAULT CHARACTER SET = utf8;

/* 创建group_list表 */
CREATE TABLE IF NOT EXISTS `ChatR_Base`.`group_list` (
  `id` INT UNSIGNED NOT NULL AUTO_INCREMENT,
  `name` VARCHAR(45)  NOT NULL,
  `create_time` DATE  NOT NULL,
  `member_num` INT  NOT NULL,
  `introduction` VARCHAR(256)  DEFAULT '群主太懒了，还没有群介绍~',
  PRIMARY KEY (`id`))
ENGINE = InnoDB
DEFAULT CHARACTER SET = utf8;

/* 创建user_info数据库 */
CREATE DATABASE IF NOT EXISTS user_info;

/* 创建group_info数据库 */
CREATE DATABASE IF NOT EXISTS group_info;
