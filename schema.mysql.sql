-- create syte users (competioin juides)

create table `site_user` (
	`uid` integer primary key auto_increment,
	`login` varchar(16) not null unique,
	`pwdhash` varchar(32) not null,
	`name` varchar(32) not null
) engine = InnoDb comment 'Site user list (competition juide command)';
insert into `site_user`    (`login`, `name`, `pwdhash`) values
('admin', 'Администратор безопастности', md5('siteadmin'));

-- create base library tables
create table `lib_climbdifficalty` (
	`id` integer primary key auto_increment,
	`label` varchar(6) not null unique
) engine = InnoDb comment 'Climbing difficalty category';

insert into `lib_climbdifficalty` (`label`) values ('4'),
('5a'),('5a+'),('5b'),('5b+'),('5c'),('5c+'),
('6a'),('6a+'),('6b'),('6b+'),('6c'),('6c+'),
('7a'),('7a+'),('7b'),('7b+'),('7c'),('7c+'),
('8a'),('8a+'),('8b'),('8b+'),('8c'),('8c+'),
('9a'),('9a+'),('9b'),('9b+'),('9c');

create table `lib_sportrange` (
	`id` integer primary key auto_increment,
	`label` varchar(15) not null unique
) engine = InnoDb comment 'Sportage ranks and ranges';

insert into `lib_sportrange` (`label`) values
('без разряда'),('3-й разряд'),('2-й разряд'),('1-й разряд'),
('КМС'),('Мастер спорта'),('Заслуженный МС'),('МС МК');

create table `lib_agegroup` (
	`id` integer primary key auto_increment,
	`label` varchar(64) not null unique,
	`gender` set ('М', 'Ж') not null default 'М,Ж',
	`min_age` integer not null,
	`max_age` integer not null,
	constraint `unq_agegroup` unique key (`gender`,`min_age`,`max_age`)
) engine = InnoDb comment 'Separate competitors from age out and gender';

insert into `lib_agegroup` (`label`,`gender`,`min_age`, `max_age`) values
('девочки','Ж','0','9'),
('мальчики','М','0','9'),
('дети','Ж,М','0','9'),
('младшие подростки девушки','Ж','10','11'),
('младшие подростки юноши','М','10','11'),
('младшие подростки','Ж,М','10','11'),
('старшие подростки девушки','Ж','12','13'),
('старшие подростки юноши','М','12','13'),
('старшие подростки','Ж,М','12','13'),
('подростки девушки','Ж','10','13'),
('подростки юноши','М','10','13'),
('подростки','Ж,М','10','13'),
('младшие девушки','Ж','14','15'),
('младшие юноши','М','14','15'),
('младшие юноши и девушки','Ж,М','14','15'),
('старшие девушки','Ж','16','17'),
('старшие юноши','М','16','17'),
('старшие юноши и девушки','Ж,М','16','17'),
('юниорки','Ж','18','19'),
('юниоры','М','18','19'),
('юниоры и юниорки','Ж,М','18','19'),
('женщины','Ж','20','44'),
('мужчины','М','20','44'),
('спортсмены','Ж,М','20','44'),
('ветераны женщины','Ж','45','99'),
('ветераны мужчины','М','45','99'),
('ветераны','Ж,М','45','99');

create table `lib_competitiontype` (
	`id` integer primary key auto_increment,
	`prefix` varchar(15) unique not null,
	`title` varchar(50) not null unique,
	`vrvscode` varchar (11) default null,
	`description` tinytext not null
) engine = InnoDb comment 'Supported competition types';
insert into `lib_competitiontype` (`prefix`, `title`, `vrvscode`, `description`) values
('bouldering_','Боулдеринг','0800011511Я','Серия коротких проблемных трасс'),
('difficulty_','Лазание на трудность','0800031511Я','Трудность. Индивидуальное лазание'),
('pairrace_','Лазание на скорость (парная гонка)','0800021511Я','Скорость. Парная гонка (Дуэль)'),
('spd_individual_','Лазание на скорость (индивидуальное лазание)','0800021511Я','Скорость. Индивидуальное лазание');

create table `lib_team` (
	`gid` integer primary key auto_increment,
	`title` varchar(50) not null unique,
	`region` varchar(50) default null
) engine = InnoDb comment 'Sport team';
insert into `lib_team`(`title`) values ('лично');

create table `lib_competitor` (
	`uid` integer primary key auto_increment,
	`name` varchar(50) not null,
	`dob` year not null,
	`gender` enum ('М','Ж') not null,
	`range` integer not null default 1,
	`team` integer not null default 1,
	key `fk_competitor_range` (`range`),
	key `fk_competitor_team` (`team`),
	index `id_competitior_name` using btree (`name`,`gender`,`dob`),
	constraint `fk_competitor_range` foreign key (`range`) references `lib_sportrange` (`id`) on update cascade on delete restrict,
	constraint `fk_competitor_team` foreign key (`team`) references `lib_team` (`gid`) on update cascade on delete restrict
) engine = InnoDb comment 'Table of competitors';

-- create competition structure common tables
create table `competition` (
	`cid` integer primary key auto_increment,
	`title` varchar(255) not null unique,
	`emblem` blob default null,
	`localtion` varchar (128) not null,
	`date` date not null,
	`type` integer not null,
	`main_referee` integer not null,
	`main_secretar` integer not null,
	`class` enum ('I класс','II класс','III класс') not null default 'III класс',
	key `fk_competition_type` (`type`),
	key `fk_competition_main_referee` (`main_referee`),
	key `fk_competition_main_secretar` (`main_secretar`),
	constraint `fk_competition_type` foreign key (`type`) references `lib_competitiontype` (`id`) on update cascade on delete restrict,
	constraint `fk_competition_main_referee` foreign key (`main_referee`) references `site_user` (`uid`) on update cascade on delete restrict,
	constraint `fk_competition_main_secretar` foreign key (`main_secretar`) references `site_user` (`uid`) on update cascade on delete restrict
) engine = InnoDb comment 'competition list';

create table `round` (
	`rid` integer primary key auto_increment comment 'round id',
	`competition` integer not null comment 'pointer to competition id',
	`type` integer not null comment 'pointer to supported competition type',
	`parent` integer default null comment 'pointer to parent round. is null that this round first in this age group',
	`agegroup` integer not null comment 'pointer to age group',
	`position` enum('Квалификация', '1/8 финала', '1/4 финала','полуфинал', 'финал', 'суперфинал') not null comment 'round position in competition structure',
	`winer_quota` integer not null comment 'quota to winner of that round. Calculated in spo intime module creation? and maybe changed in competition process',
	key `fk_round_competition` (`competition`),
	key `fk_round_type` (`type`),
	key `fk_round_parent` (`parent`),
	key `fk_round_agegroup` (`agegroup`),
	constraint `fk_round_competition` foreign key (`competition`) references `competition` (`cid`) on update cascade on delete cascade,
	constraint `fk_round_type` foreign key (`type`) references `lib_competitiontype` (`id`) on update cascade on delete restrict,
	constraint `fk_round_parent` foreign key (`parent`) references `round` (`rid`) on update cascade on delete cascade,
	constraint `fk_round_agegroup` foreign key (`agegroup`) references `lib_agegroup` (`id`) on update cascade on delete restrict
) engine InnoDb comment 'competition round list';

create table `competition_member` (
	`id` integer primary key auto_increment,
	`round` integer not null comment 'pointer to round in competition',
	`cid` integer not null comment 'pointer to competitor',
	`startposition` integer default null comment 'start position after draw',
	key `fk_member_round` (`round`),
	key `fk_member_cid` (`cid`),
	constraint `fk_member_round` foreign key (`round`) references `round` (`rid`) on update cascade on delete restrict,
	constraint `fk_member_cid` foreign key (`cid`) references `lib_competitor` (`uid`) on update cascade on delete restrict
) engine = InnoDb comment 'member list from round in current compettion';

create table `route` (
	`id` integer primary key auto_increment,
	`round` integer not null,
	`title` varchar(32),
	`hight` float not null,
	key `fk_route_round` (`round`),
	constraint `fk_route_round` foreign key (`round`) references `round` (`rid`) on update cascade on delete cascade,
	constraint `uk_round_route` unique (`round`,`title`)
) engine = InnoDb comment 'route to competition';

create table `route_point` (
	`id` integer primary key auto_increment,
	`route` integer not null,
	`num` varchar(5) not null comment 'point label (number) in scheme',
	`hight` float not null,
	key `fk_route_point` (`route`),
	constraint `fk_route point` foreign key (`route`) references `route` (`id`) on update cascade on delete cascade,
	constraint `uk_route_point` unique (`route`,`num`)
) engine = InnoDb comment 'climb point in route';

-- pre action managment section
create table `preaction` (
	`id` integer primary key auto_increment,
	`competition` integer not null,
	`team` integer not null,
	`published` timestamp not null default current_timestamp,
	`state` enum ('Не обработано', 'Принято', 'Отклонено', 'Отозванно') not null default 'Не обработано',
	`representative` varchar(32) default null,
	`requestaddr` varchar(128) not null comment 'e-mail to requestrequestaddr',
	`acesscode` varchar(32) not null comment 'access code to backcall preaction',
	key `fk_preaction_competition` (`competition`),
	key `fk_preaction_team` (`team`),
	constraint `fk_preaction_competition` foreign key (`competition`) references `competition` (`cid`) on update cascade on delete cascade,
	constraint `fk_preaction_team` foreign key (`team`) references `lib_team` (`gid`) on update cascade on delete cascade
) engine = InnoDb comment 'Preaction list';

create table `preaction_member` (
	`id` integer(32) primary key auto_increment,
	`preaction` integer not null,
	`round` integer not null,
	`uid` integer not null,
	`cpecialnote` varchar(100) default null,
	key `fk_preaction_parent` (`preaction`),
	key `fk_preaction_round` (`round`),
	key `fk_preaction_uid` (`uid`),
	constraint `fk_preaction_parent` foreign key (`preaction`) references `preaction` (`id`) on update cascade on delete cascade,
	constraint `fk_preaction_round` foreign key (`round`) references `round` (`rid`) on update cascade on delete restrict,
	constraint `fk_preaction_uid` foreign key (`uid`) references `lib_competitor` (`uid`) on update cascade on delete restrict,
	constraint `uk_preaction_member` unique (`round`, `uid`)
) engine = InnoDb comment 'Members of preaction';
