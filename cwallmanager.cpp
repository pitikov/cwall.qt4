#include "cwallmanager.h"
#include <QMessageBox>

CWallManager::CWallManager(QWidget* parent, Qt::WindowFlags flags)
: QMainWindow(parent, flags)
, ui(new Ui::CWallManager)
, dialogDatabase(new DialogDatabase(this))
, host(QString())
, base(QString())
, user(QString())
, passwd(QString())
, port(3306)
, cwallbase(QSqlDatabase::addDatabase("QMYSQL", "cwall"))
{
	ui->setupUi(this);

	setConnections();

	ui->actionManagerDbConnect->setChecked(DialogConfigure::cfg()->defaultDatabase(&host, &base, &user, &passwd, &port));

}

void CWallManager::setConnections()
{
	connect(ui->actionManagerDatabase, SIGNAL(activated()), dialogDatabase, SLOT(exec()));
	connect(ui->actionManagerConfiguration, SIGNAL(activated()), DialogConfigure::cfg(this), SLOT(exec()));
	connect(ui->actionManagerDbConnect, SIGNAL(toggled(bool)), this, SLOT(db_connect(bool)));
	connect(ui->actionManagerExit, SIGNAL(activated()), this, SLOT(close()));

	connect(dialogDatabase, SIGNAL(accepted()), this, SLOT(db_cfg_update()));
}

bool CWallManager::event(QEvent* event)
{
	return QMainWindow::event(event);
}

CWallManager::~CWallManager()
{
	delete dialogDatabase;
}

void CWallManager::db_connect(bool is_connect)
{
	ui->actionManagerDbConnect->blockSignals(true);
	if (is_connect) {
		cwallbase.setHostName(host);
		cwallbase.setDatabaseName(base);
		cwallbase.setUserName(user);
		cwallbase.setPassword(passwd);
		cwallbase.setPort(port);

		if (dialogDatabase->validate_dbstruct()) {
			is_connect = cwallbase.open();

			// TODO activated objects from database open
		} else {
			is_connect = false;
			QMessageBox::critical(this, windowTitle(), tr("Uncorrect or damaged structure in default database") + ": " + user +"@" + base, QMessageBox::Close);
		}
	} else {
		// TODO close and diactivaed all SQL-requered objects

		cwallbase.close();
	}
	ui->menuCompetition->setEnabled(is_connect);
	ui->toolBarCompetition->setEnabled(is_connect);
	ui->actionManagerDbConnect->setChecked(is_connect);
	ui->actionManagerDbConnect->blockSignals(false);
}

void CWallManager::db_cfg_update()
{
	ui->actionManagerDbConnect->setChecked(DialogConfigure::cfg()->defaultDatabase(&host, &base, &user, &passwd, &port));
	if (ui->actionManagerDbConnect->isChecked()) {
		db_connect(false);
		db_connect(true);
	}
}


#include "cwallmanager.moc"