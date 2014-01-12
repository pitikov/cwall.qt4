#ifndef USER_H
#define USER_H

#include <QObject>

class User : public QObject
{
  Q_OBJECT

public:
	typedef enum Role {
		None = 0x00,
		RaceJuide = 0x01,
		MainSecretar = 0x03,
		MainReferee = 0x05,
		Admin = 0x08,
		Grand = None | RaceJuide | MainSecretar | MainReferee
	} Role;

  ~User();
	int uid();
	static User *current( int uid = -1 );
	bool isSuperuser();
	Role access(int competition);
	QString name();
	QString login();

private:
	explicit User(int uid);
	static User *self_;
	int uid_;
};

#endif // USER_H
