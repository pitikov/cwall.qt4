#include "user.h"
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QSqlQueryModel>
#include <QSqlRecord>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <QDebug>

User *User::self_ = NULL;

User::User(int uid)
	: QObject()
	, uid_(uid)
{
}

User::~User()
{
	self_ = NULL;
}

User* User::current(int uid)
{
	if (!self_) {
		self_ = new User(uid);
	}
	return self_;
}

bool User::isSuperuser()
{
	return uid_ == 1;
}

int User::uid()
{
	return uid_;
}

User::Role User::access(int competition)
{
	// TODO test me from real data
	if (isSuperuser()) return Grand;

	int ret = None;
	QSqlQueryModel modelCompetition(this);
	QSqlQueryModel modelJuide(this);
	modelCompetition.setQuery("select * from `competition` where `cid`="+QString::number(competition)+";", QSqlDatabase::database("cwall"));
	modelCompetition.query().exec();

	if (modelCompetition.rowCount()>0) {
		if (modelCompetition.record(0).value("main_referee").toInt() == uid_) ret |= MainReferee;
		if (modelCompetition.record(0).value("main_secretar").toInt() == uid_) ret |= MainSecretar;
	}

	modelJuide.setQuery("select * from `competioin_juide` where `competition`="+QString::number(competition)+";", QSqlDatabase::database("cwall"));
	modelJuide.query().exec();

	for (int id = 0; id < modelJuide.rowCount(); id++) {
		if (modelJuide.record(id).value("uid") == uid_) ret |= RaceJuide;
	}

	return (Role)ret;
}

QString User::login()
{
	QSqlTableModel userlist(this, QSqlDatabase::database("cwall"));
	userlist.setTable("site_user");
	userlist.select();
	for (int row = 0; row < userlist.rowCount(); row++) {
		if (userlist.record(row).value("uid").toInt() == uid_) {
			return userlist.record(row).value("login").toString();
		}
	}
	return QString();
}

QString User::name()
{
	QSqlTableModel userlist(this, QSqlDatabase::database("cwall"));
	userlist.setTable("site_user");
	userlist.select();
	for (int row = 0; row < userlist.rowCount(); row++) {
		if (userlist.record(row).value("uid").toInt() == uid_) {
			return userlist.record(row).value("name").toString();
		}
	}
	return QString();
}



#include "user.moc"
