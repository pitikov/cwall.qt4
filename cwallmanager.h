#ifndef CWALLMANAGER_H
#define CWALLMANAGER_H

#include <QMainWindow>
#include <qmdisubwindow.h>
#include <QtSql>

#include "ui_cwall_manager.h"
#include "ui_dialog_auth.h"
#include "dialogdatabase.h"
#include "dialogconfigure.h"
#include "formlibraryeditor.h"

namespace Ui {
	class CWallManager;
	class DialogAuth;
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
	Ui::DialogAuth	*ui_auth;
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

	void db_open_success();
	void db_close();

};

#endif // CWALLMANAGER_H