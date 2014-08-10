/*
Navicat MySQL Data Transfer

Source Server         : HomeSRV
Source Server Version : 50538
Source Host           : localhost:3306
Source Database       : mangos

Target Server Type    : MYSQL
Target Server Version : 50538
File Encoding         : 65001

Date: 2014-08-10 06:57:23
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for refreshitems
-- ----------------------------
DROP TABLE IF EXISTS `refreshitems`;
CREATE TABLE `refreshitems` (
  `class` tinyint(2) unsigned DEFAULT NULL,
  `itemid` int(10) unsigned DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
