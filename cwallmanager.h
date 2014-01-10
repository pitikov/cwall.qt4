#ifndef CWALLMANAGER_H
#define CWALLMANAGER_H

#include <QMainWindow>

#include "ui_cwall_manager.h"
#include "dialogdatabase.h"
#include "dialogconfigure.h"

namespace Ui {
	class CWallManager;
}

class CWallManager : public QMainWindow
{
	Q_OBJECT
public:
	explicit CWallManager(QWidget* parent = 0, Qt::WindowFlags flags = 0);
	~CWallManager();

protected:
	virtual bool event(QEvent* event);
	void setConnections();

	Ui::CWallManager 	*ui;
	DialogDatabase	*dialogDatabase;
	QString		host;
	QString		base;
	QString  	user;
	QString		passwd;
	int 			port;
	QSqlDatabase	cwallbase;

public slots:
	void db_connect(bool is_connect);
	void db_cfg_update();

};

#endif // CWALLMANAGER_H
