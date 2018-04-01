DROP TABLE IF EXISTS D;
CREATE TABLE D (
	     yxh CHAR(2) NOT NULL,
	     mc VARCHAR(50) NOT NULL,
       dz VARCHAR(50) NOT NULL,
       lxdh CHAR(8) NOT NULL,
       PRIMARY KEY(yxh)
) ENGINE = INNODB;

INSERT INTO D(yxh, mc, dz, lxdh) VALUES
('01', '计算机学院', '上大东校区三号楼', '65347567'),
('02', '通讯学院', '上大东校区二号楼', '65341234'),
('03', '材料学院', '上大东校区四号楼', '65347890');

DROP TABLE IF EXISTS S;
CREATE TABLE S (
       xh CHAR(4) NOT NULL,
       xm VARCHAR(50) NOT NULL,
       xb ENUM('男', '女') DEFAULT '女' NOT NULL,
       csrq DATE NOT NULL,
       jg VARCHAR(10) NOT NULL,
       sjhm CHAR(11) NOT NULL,
       yxh CHAR(2),
       PRIMARY KEY(xh),
       FOREIGN KEY(yxh) REFERENCES D(yxh) ON DELETE NO ACTION
) ENGINE = INNODB AUTO_INCREMENT = 1101;

INSERT INTO S(xm, xb, csrq, jg, sjhm, yxh) VALUES
('1101', '李明', '男', '1993-03-06', '上海', '13613005486', '02'),
('1102', '刘晓明', '男', '1992-12-08', '安徽', '18913457890', '01'),
('1103', '张颖', '女', '1993-01-05', '江苏', '18826490423', '01'),
('1104', '刘晶晶', '女', '1994-11-06', '上海', '13331934111', '01'),
('1105', '刘成刚', '男', '1991-06-07', '上海', '18015872567', '01'),
('1106', '李二丽', '女', '1993-05-04', '江苏', '18107620945', '01'),
('1107', '张晓峰', '男', '1992-08-16', '浙江', '13912341078', '01');

DROP TABLE IF EXISTS T;
CREATE TABLE T (
       gh CHAR(4) NOT NULL,
       xm VARCHAR(50) NOT NULL,
       xb ENUM('男', '女') DEFAULT '女' NOT NULL,
       csrq DATE NOT NULL,
       xl ENUM('教授', '副教授', '讲师') DEFAULT '讲师' NOT NULL,
       jbgz FLOAT(7,2) NOT NULL,
       yxh CHAR(2) NOT NULL,
       PRIMARY KEY(gh),
       FOREIGN KEY(yxh) REFERENCES D(yxh) ON DELETE NO ACTION
) ENGINE = INNODB;

INSERT INTO T(gh, xm, xb, csrq, xl, jbgz, yxh) VALUES
('0101', '陈迪茂', '男', '1973-03-06', '副教授', 3567.00, '01'),
('0102', '马小红', '女', '1972-12-08', '讲师', 2845.00, '01'),
('0201', '张心颖', '女', '1960-01-05', '教授', 4200.00, '02'),
('0103', '吴宝刚', '男', '1980-11-06', '讲师', 2554.00, '01');

DROP TABLE IF EXISTS C;
CREATE TABLE C (
	     kh CHAR(8) NOT NULL,
       km VARCHAR(20) NOT NULL,
       xf TINYINT(1) UNSIGNED NOT NULL,
       xs TINYINT(2) UNSIGNED NOT NULL,
       yxh CHAR(2) NOT NULL,
       PRIMARY KEY(kh),
       FOREIGN KEY(yxh) REFERENCES D(yxh) ON DELETE NO ACTION
) ENGINE = INNODB;

INSERT INTO C(kh, km, xf, xs, yxh) VALUES
('08305001', '离散数学', 4, 40, '01'),
('08305002', '数据库原理', 4, 50, '01'),
('08305003', '数据结构', 4, 50, '01'),
('08305004', '系统结构', 6, 60, '01'),
('08301001', '分子物理学', 4, 40, '03'),
('08302001', '通信学', 3, 30, '02');

DROP TABLE IF EXISTS O;
CREATE TABLE O (
	     xq CHAR(12) NOT NULL,
       kh CHAR(8) NOT NULL,
       gh CHAR(4) NOT NULL,
       sksj VARCHAR(20) NOT NULL,
       PRIMARY KEY(xq, kh, gh),
       FOREIGN KEY(kh) REFERENCES C(kh) ON DELETE CASCADE,
       FOREIGN KEY(gh) REFERENCES T(gh) ON DELETE CASCADE
) ENGINE = INNODB;

INSERT INTO O(xq, kh, gh, sksj) VALUES
('2012-2013 秋季', '08305001', '0103', '星期三 5-8'),
('2012-2013 冬季', '08305002', '0101', '星期三 1-4'),
('2012-2013 冬季', '08305002', '0102', '星期三 1-4'),
('2012-2013 冬季', '08305002', '0103', '星期三 1-4'),
('2012-2013 冬季', '08305003', '0102', '星期五 5-8'),
('2013-2014 秋季', '08305004', '0101', '星期二 1-4'),
('2013-2014 秋季', '08305001', '0102', '星期一 5-8'),
('2013-2014 冬季', '08302001', '0201', '星期一 5-8');

DROP TABLE IF EXISTS E;
CREATE TABLE E (
	     xh CHAR(4) NOT NULL,
       xq CHAR(12) NOT NULL,
       kh CHAR(8) NOT NULL,
       gh CHAR(4) NOT NULL,
       pscj TINYINT UNSIGNED,
       kscj TINYINT UNSIGNED,
       zpcj TINYINT UNSIGNED,
       PRIMARY KEY(xh, xq, kh, gh),
       FOREIGN KEY(xh) REFERENCES S(xh) ON DELETE CASCADE,
       FOREIGN KEY(xq, kh, gh) REFERENCES O(xq, kh, gh) ON DELETE CASCADE
) ENGINE = INNODB;

INSERT INTO E(xh, xq, kh, gh, pscj, kscj, zpcj) VALUES
('1101', '2012-2013 秋季', '08305001', '0103', 60, 60, 60),
('1102', '2012-2013 秋季', '08305001', '0103', 87, 87, 87),
('1102', '2012-2013 冬季', '08305002', '0101', 82, 82, 82),
('1102', '2013-2014 秋季', '08305004', '0101', null, null, null),
('1103', '2012-2013 秋季', '08305001', '0103', 56, 56, 56),
('1103', '2012-2013 冬季', '08305002', '0102', 75, 75, 75),
('1103', '2012-2013 冬季', '08305003', '0102', 84, 84, 84),
('1103', '2013-2014 秋季', '08305001', '0102', null, null, null),
('1103', '2013-2014 秋季', '08305004', '0101', null, null, null),
('1104', '2012-2013 秋季', '08305001', '0103', 74, 74, 74),
('1104', '2013-2014 冬季', '08302001', '0201', null, null, null),
('1106', '2012-2013 秋季', '08305001', '0103', 85, 85, 85),
('1106', '2012-2013 冬季', '08305002', '0103', 66, 66, 66),
('1107', '2012-2013 秋季', '08305001', '0103', 90, 90, 90),
('1107', '2012-2013 冬季', '08305003', '0102', 79, 79, 79),
('1107', '2013-2014 秋季', '08305004', '0101', null, null, null);
