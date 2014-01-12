#include "user.h"
#include <QSqlDatabase>

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


#include "user.moc"
