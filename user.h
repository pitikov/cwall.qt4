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
		Secretar = 0x02,
		MainRefere = 0x04,
		Grand = None | RaceJuide | Secretar | MainRefere
	} Role;

  ~User();
	int uid();
	static User *current( int uid = -1 );
	bool isSuperuser();


private:
	explicit User(int uid);
	static User *self_;
	int uid_;
};

#endif // USER_H
