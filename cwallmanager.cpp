#include "cwallmanager.h"
#include <QMessageBox>

#include <QCryptographicHash>

CWallManager::CWallManager(QWidget* parent, Qt::WindowFlags flags)
: QMainWindow(parent, flags)
, ui(new Ui::CWallManager)
, ui_auth(new Ui::DialogAuth)
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
			if (is_connect) {
				QDialog dialog_auth;
				ui_auth->setupUi(&dialog_auth);
				ui_auth->lineEdit->clear();
				ui_auth->comboBox->clear();

				QSqlQueryModel sql_model_user;
				sql_model_user.setQuery("select * from site_user", cwallbase);

				for (int uid = 0; uid < sql_model_user.rowCount(); uid++) {
					ui_auth->comboBox->addItem(sql_model_user.record(uid).value("login").toString(), sql_model_user.record(uid).value("record").toInt());
				}
				if (dialog_auth.exec() == QDialog::Accepted) {

					if (QCryptographicHash::hash(ui_auth->lineEdit->text().toLatin1(), QCryptographicHash::Md5).toHex() == sql_model_user.record(ui_auth->comboBox->currentIndex()).value("pwdhash").toString()) {

						// TODO activated objects from database open
					} else {
						QMessageBox::warning(this, dialog_auth.windowTitle(), tr("Uncorrect password"));
						cwallbase.close();
						is_connect = false;
					}
				} else {
					cwallbase.close();
					is_connect = false;
				}
			}
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