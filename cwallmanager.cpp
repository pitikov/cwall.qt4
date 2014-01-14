#include "cwallmanager.h"
#include "user.h"
#include <QMessageBox>
#include <QMdiSubWindow>

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

	db_connect(false);
	setConnections();
	ui->actionManagerDbConnect->setChecked(DialogConfigure::cfg()->defaultDatabase(&host, &base, &user, &passwd, &port));
	configUpdate();	
}

void CWallManager::setConnections()
{
	connect(ui->actionManagerDatabase, SIGNAL(activated()), dialogDatabase, SLOT(exec()));
	connect(ui->actionManagerConfiguration, SIGNAL(activated()), DialogConfigure::cfg(this), SLOT(exec()));
	connect(ui->actionManagerDbConnect, SIGNAL(toggled(bool)), this, SLOT(db_connect(bool)));
	connect(ui->actionManagerExit, SIGNAL(activated()), this, SLOT(close()));

	connect(ui->actionLibrary_static_data, SIGNAL(activated()), this, SLOT(mdi_window_show()));

	connect(dialogDatabase, SIGNAL(accepted()), this, SLOT(db_cfg_update()));
	connect(DialogConfigure::cfg(), SIGNAL(accepted()), this, SLOT(configUpdate()));
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

		is_connect = cwallbase.open();
		if (is_connect) {

			if (dialogDatabase->validate_dbstruct(&cwallbase)) {
				QDialog dialog_auth;
				ui_auth->setupUi(&dialog_auth);
				ui_auth->lineEdit->clear();
				ui_auth->comboBox->clear();
				
				connect(ui_auth->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(db_auth_validate(int)));

				QSqlQueryModel sql_model_user;
				sql_model_user.setQuery("select * from site_user", cwallbase);

				ui_auth->comboBox->addItem(tr("Select database account"), 0);
				for (int uid = 0; uid < sql_model_user.rowCount(); uid++) {
					ui_auth->comboBox->addItem(sql_model_user.record(uid).value("login").toString(), sql_model_user.record(uid).value("uid").toInt());
				}
				ui_auth->comboBox->setCurrentIndex(0);
				db_auth_validate(ui_auth->comboBox->currentIndex());
				if (dialog_auth.exec() == QDialog::Accepted) {
					int record = 0;
					for (int recid = 0; recid < sql_model_user.rowCount(); recid++) {
						if (sql_model_user.record(recid).value("uid").toInt() == ui_auth->comboBox->itemData(ui_auth->comboBox->currentIndex()).toInt()) {
							record = recid;
							break;
						}
					}

					if (QCryptographicHash::hash(ui_auth->lineEdit->text().toLatin1(), QCryptographicHash::Md5).toHex() == sql_model_user.record(record).value("pwdhash").toString()) {
						User::current(ui_auth->comboBox->itemData(ui_auth->comboBox->currentIndex()).toInt());
						db_open_success();
					} else {
						QMessageBox::warning(this, dialog_auth.windowTitle(), tr("Uncorrect password"));
						cwallbase.close();
						is_connect = false;
					}
					

				} else {
					cwallbase.close();
					is_connect = false;
				}
			} else {
				is_connect = false;
				QMessageBox::critical(this, windowTitle(), tr("Uncorrect or damaged structure in default database") + ": " + user +"@" + base, QMessageBox::Close);
			}
		}
	} else {
		ui->mdiArea->closeAllSubWindows();
		db_close();
		cwallbase.close();
	}
	ui->menuCompetition->setEnabled(is_connect);
	ui->toolBarCompetition->setEnabled(is_connect);
	ui->actionManagerDbConnect->setChecked(is_connect);
	ui->actionLibrary_static_data->setEnabled(is_connect);
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

void CWallManager::db_close()
{
	// TODO close and diactivaed all SQL-requered objects

	delete User::current();
}

void CWallManager::db_open_success()
{
	// TODO activated objects from database open

}

void CWallManager::mdi_window_show(bool status)
{
	if (sender()->objectName() == "actionLibrary_static_data") {
		QWidget *tgt = findSubWindow("FormLibraryEditor");
		if ( !tgt ) ui->mdiArea->addSubWindow(new FormLibraryEditor(this))->show();
		else tgt->showNormal();
	}
	// TODO in this point set recieving for add or delete subWindow to mdiArea
}

QWidget *CWallManager::findSubWindow(QString objectName)
{
	for (int id = 0; id < ui->mdiArea->subWindowList().count(); id++) {
		if (ui->mdiArea->subWindowList().at(id)->widget()->objectName() == objectName)
			return ui->mdiArea->subWindowList().at(id);
	}
	return NULL;
}

void CWallManager::configUpdate()
{
	ui->actionHelpRules->setVisible(DialogConfigure::cfg()->rulseAccess());
}

void CWallManager::db_auth_validate(int index)
{
	bool is_enable = false;
	if (ui_auth->comboBox->count() >= index) {
		if (ui_auth->comboBox->itemData(index).toInt() > 0) is_enable = true;
	}
	ui_auth->pushButtonLogin->setEnabled(is_enable);
}

#include "cwallmanager.moc"