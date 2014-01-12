#include "dialogdatabase.h"
#include "dialogconfigure.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QLineEdit>
#include <QMessageBox>
#include <QFile>

#include <QDebug>

DialogDatabase::DialogDatabase(QWidget* parent)
: QDialog(parent)
, ui(new Ui::DialogDatabase)
, ui_hostname(new Ui::DialogHostname)
, ui_database(new Ui::DialogDbCreate)
, ui_rebuild(new Ui::DialogRebuild)
, dialog_hostname(new QDialog(this))
, dialog_database(new QDialog(this))
, dialog_rebuild(new QDialog(0))
, model(new QStandardItemModel(0, 1, this))
, actionHostAdd(new QAction(tr("Add host"), this))
, actionHostDelete(new QAction(tr("Delete host"), this))
, actionDbAdd(new QAction(tr("Add database"), this))
, actionDbDelete(new QAction(tr("Delete database"), this))
, actionDbSetDefault(new QAction(tr("Set default"), this))
, actionDbTest(new QAction(tr("Test connection"), this))
, actionDbRebuild(new QAction(tr("Rebuild"), this))
, DbPasswordRole(Qt::UserRole)
, DbDefaultRole(Qt::UserRole+1)
, DbBaseRole(Qt::UserRole+2)
, DbUserRole(Qt::UserRole+3)
{
	ui->setupUi(this);

	ui_hostname->setupUi(dialog_hostname);
	ui_database->setupUi(dialog_database);
	ui_rebuild->setupUi(dialog_rebuild);

	ui->treeViewDatabases->setModel(model);
	ui->treeViewDatabases->header()->hide();

	ui->treeViewDatabases->setContextMenuPolicy(Qt::ActionsContextMenu);
	ui->treeViewDatabases->addAction(actionHostAdd);
	ui->treeViewDatabases->addAction(actionHostDelete);
	QAction *actionSeparate = new QAction("", this);
	actionSeparate->setSeparator(true);
	ui->treeViewDatabases->addAction(actionSeparate);
	ui->treeViewDatabases->addAction(actionDbAdd);
	ui->treeViewDatabases->addAction(actionDbDelete);
	ui->treeViewDatabases->addAction(actionDbSetDefault);
	ui->treeViewDatabases->addAction(actionDbTest);
	ui->treeViewDatabases->addAction(actionDbRebuild);

	connect(actionHostAdd, SIGNAL(activated()), SLOT(hostnameAdd()));
	connect(actionHostDelete, SIGNAL(activated()), SLOT(hostnameDelete()));
	connect(actionDbAdd, SIGNAL(activated()), SLOT(databaseAdd()));
	connect(actionDbDelete, SIGNAL(activated()), SLOT(databaseDelete()));
	connect(actionDbSetDefault, SIGNAL(activated()), SLOT(databaseSetDefault()));
	connect(actionDbTest, SIGNAL(activated()), SLOT(databaseTest()));
	connect(actionDbRebuild, SIGNAL(activated()), SLOT(databaseRebuild()));

	connect(ui->pushButtonHostAdd, SIGNAL(clicked()), SLOT(hostnameAdd()));
	connect(ui->pushButtonHostdelete, SIGNAL(clicked()), SLOT(hostnameDelete()));
	connect(ui->pushButtonDbAdd, SIGNAL(clicked()), SLOT(databaseAdd()));
	connect(ui->pushButtonDbDelete, SIGNAL(clicked()), SLOT(databaseDelete()));
	connect(ui->pushButtonDbSetDefault, SIGNAL(clicked()), SLOT(databaseSetDefault()));
	connect(ui->pushButtonDbTest, SIGNAL(clicked()), SLOT(databaseTest()));
	connect(ui->pushButtonDbRebuild, SIGNAL(clicked()), SLOT(databaseRebuild()));

	connect(ui->treeViewDatabases, SIGNAL(doubleClicked(QModelIndex)), SLOT(databaseSetDefault(QModelIndex)));

	connect(ui_hostname->lineEdit, SIGNAL(textChanged(QString)), SLOT(validate_hostname(QString)));

	connect(ui_database->lineEditDatabase, SIGNAL(textChanged(QString)), SLOT(validate_database()));
	connect(ui_database->lineEditUserName, SIGNAL(textChanged(QString)), SLOT(validate_database()));

	indexChanged(ui->treeViewDatabases->currentIndex());
}

DialogDatabase::~DialogDatabase()
{
	delete model;
	delete dialog_rebuild;
	delete dialog_database;
	delete dialog_hostname;
	delete ui_rebuild;
	delete ui_database;
	delete ui_hostname;
	delete ui;
}

void DialogDatabase::accept()
{
	DialogConfigure::cfg()->settings()->remove(DbSettings);
	DialogConfigure::cfg()->settings()->beginWriteArray(DbSettings);

	for (int chost = 0; chost < model->rowCount(); chost++) {
		DialogConfigure::cfg()->settings()->setArrayIndex(chost);
		DialogConfigure::cfg()->settings()->setValue("hostname", model->item(chost)->text());

		DialogConfigure::cfg()->settings()->beginWriteArray(DbItems);
		for (int cdbase = 0; cdbase < model->item(chost)->rowCount(); cdbase++) {
			DialogConfigure::cfg()->settings()->setArrayIndex(cdbase);

			DialogConfigure::cfg()->settings()->setValue("database", model->item(chost)->child(cdbase)->data(DbBaseRole).toString());
			DialogConfigure::cfg()->settings()->setValue("user", model->item(chost)->child(cdbase)->data(DbUserRole).toString());
			DialogConfigure::cfg()->settings()->setValue("password", model->item(chost)->child(cdbase)->data(DbPasswordRole).toString());
			DialogConfigure::cfg()->settings()->setValue("is_default", model->item(chost)->child(cdbase)->data(DbDefaultRole).toBool());
		}
		DialogConfigure::cfg()->settings()->endArray();
	}

	DialogConfigure::cfg()->settings()->endArray();

	DialogConfigure::cfg()->settings()->sync();

	QDialog::accept();
}

void DialogDatabase::reject()
{
	QDialog::reject();
}

int DialogDatabase::exec()
{
	model->clear();
	DialogConfigure::cfg()->settings()->sync();

	int hosts = DialogConfigure::cfg()->settings()->beginReadArray(DbSettings);

	for (int chost = 0; chost < hosts; chost++) {
		DialogConfigure::cfg()->settings()->setArrayIndex(chost);
		QStandardItem *host = new QStandardItem(DialogConfigure::cfg()->settings()->value("hostname", "localhost").toString());
		host->setData(QIcon::fromTheme("network-server"), Qt::DecorationRole);
		host->setEditable(false);
		model->appendRow(host);

		int dbases = DialogConfigure::cfg()->settings()->beginReadArray(DbItems);

		for (int cdbase = 0; cdbase < dbases; cdbase++) {
			DialogConfigure::cfg()->settings()->setArrayIndex(cdbase);

			QString dbase = DialogConfigure::cfg()->settings()->value("database", "").toString();
			QString username = DialogConfigure::cfg()->settings()->value("user", "").toString();
			QString password = DialogConfigure::cfg()->settings()->value("password", "").toString();
			bool is_default = DialogConfigure::cfg()->settings()->value("is_default", false).toBool();

			if (!dbase.isEmpty() & !username.isEmpty()) {
				QStandardItem *dbitem = new QStandardItem(QIcon::fromTheme("server-database"), username+"@"+dbase);
				dbitem->setData(password, DbPasswordRole);
				dbitem->setData(username, DbUserRole);
				dbitem->setData(dbase, DbBaseRole);
				dbitem->setData(is_default, DbDefaultRole);
				dbitem->setEditable(false);
				model->item(chost)->appendRow(dbitem);
				if (is_default) {
					dbitem->setData(QIcon::fromTheme("dialog-ok-apply"), Qt::DecorationRole);
					ui->treeViewDatabases->setCurrentIndex(dbitem->index());
					indexChanged(dbitem->index());
				}
			}
		}

		DialogConfigure::cfg()->settings()->endArray();

	}
	DialogConfigure::cfg()->settings()->endArray();

	ui->treeViewDatabases->expandAll();

	return QDialog::exec();
}

void DialogDatabase::indexChanged(QModelIndex index)
{
	bool isHost = index.isValid() & !index.parent().isValid();
	bool isDatabase = index.parent().isValid();

	actionHostAdd->setEnabled(true);
	ui->pushButtonHostAdd->setEnabled(true);
	actionHostDelete->setEnabled(isHost);
	ui->pushButtonHostdelete->setEnabled(isHost);
	actionDbAdd->setEnabled(isHost );
	ui->pushButtonDbAdd->setEnabled(isHost);
	actionDbDelete->setEnabled(isDatabase);
	ui->pushButtonDbDelete->setEnabled(isDatabase);
	actionDbSetDefault->setEnabled(isDatabase);
	ui->pushButtonDbSetDefault->setEnabled(isDatabase);
	actionDbTest->setEnabled(isDatabase);
	ui->pushButtonDbTest->setEnabled(isDatabase);
	actionDbRebuild->setEnabled(isDatabase);
	ui->pushButtonDbRebuild->setEnabled(isDatabase);
}

void DialogDatabase::hostnameAdd()
{
	ui_hostname->lineEdit->clear();
	validate_hostname(ui_hostname->lineEdit->text());
	if (dialog_hostname->exec() == QDialog::Accepted) {
		QStandardItem *item = new QStandardItem(ui_hostname->lineEdit->text());
		item->setData(QIcon::fromTheme ("network-server"), Qt::DecorationRole);
		item->setEditable(false);
		model->appendRow(item);
		ui->treeViewDatabases->setCurrentIndex(item->index());
		indexChanged(item->index());
	}
}

void DialogDatabase::hostnameDelete()
{
	if (QMessageBox::warning(this, windowTitle(), tr("Delete current server item?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes) {
		model->removeRow(ui->treeViewDatabases->currentIndex().row());
	}
	indexChanged(ui->treeViewDatabases->currentIndex());
}

void DialogDatabase::databaseAdd()
{
	ui_database->lineEditDatabase->clear();
	ui_database->lineEditPassword->clear();
	ui_database->lineEditUserName->clear();
	validate_database();
	if (dialog_database->exec() == QDialog::Accepted) {
		QStandardItem *item = new QStandardItem(ui_database->lineEditUserName->text()+"@"+ui_database->lineEditDatabase->text());
		item->setData(ui_database->lineEditPassword->text(), DbPasswordRole);
		item->setData(false, DbDefaultRole);
		item->setData(ui_database->lineEditDatabase->text(), DbBaseRole);
		item->setData(ui_database->lineEditUserName->text(), DbUserRole);
		item->setData(QIcon::fromTheme ("server-database"), Qt::DecorationRole);
		item->setEditable(false);
		model->item(ui->treeViewDatabases->currentIndex().row())->appendRow(item);
		ui->treeViewDatabases->expand(item->parent()->index());

		bool is_default = false;
		for (int hostid = 0; hostid < model->rowCount(); hostid++) {
			for (int baseid = 0; baseid < model->item(hostid)->rowCount(); baseid++) {
				is_default = model->item(hostid)->child(baseid)->data(DbDefaultRole).toBool()?true:is_default;
			}
		}
		if (!is_default)  {
			item->setData(true, DbDefaultRole);
			item->setData(QIcon::fromTheme ("dialog-ok-apply"), Qt::DecorationRole);
		}
	}
}

void DialogDatabase::databaseDelete()
{
	QModelIndex index = ui->treeViewDatabases->currentIndex().parent();
	if (QMessageBox::warning(this, windowTitle(), tr("Delete current database item?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes) {
		model->item(ui->treeViewDatabases->currentIndex().parent().row())->removeRow(ui->treeViewDatabases->currentIndex().row());
	}
	ui->treeViewDatabases->setCurrentIndex(index);
	indexChanged(index);
}

void DialogDatabase::databaseSetDefault(QModelIndex index)
{
	if ((sender() == actionDbSetDefault)|(sender() == ui->pushButtonDbSetDefault))
	{
		index = ui->treeViewDatabases->currentIndex();
	}
	if (index.parent().isValid()) {
		for (int hostid = 0; hostid < model->rowCount(); hostid++) {
			for (int baseid = 0; baseid < model->item(hostid)->rowCount(); baseid++) {
				model->item(hostid)->child(baseid)->setData(false, DbDefaultRole);
				model->item(hostid)->child(baseid)->setData(QIcon::fromTheme ("server-database"), Qt::DecorationRole);
			}
		}
		model->item(ui->treeViewDatabases->currentIndex().parent().row())->child(ui->treeViewDatabases->currentIndex().row())->setData(true, DbDefaultRole);
		model->item(ui->treeViewDatabases->currentIndex().parent().row())->child(ui->treeViewDatabases->currentIndex().row())->setData(QIcon::fromTheme ("dialog-ok-apply"), Qt::DecorationRole);
	}
}

void DialogDatabase::databaseTest()
{
	QSqlDatabase testdb = QSqlDatabase::addDatabase("QMYSQL", "testdb");

	QModelIndex db_index = ui->treeViewDatabases->currentIndex();
	testdb.setDatabaseName(model->data(db_index, DbBaseRole).toString());
	testdb.setHostName(model->data(db_index.parent(), Qt::DisplayRole).toString());
	testdb.setUserName(model->data(db_index, DbUserRole).toString());
	testdb.setPassword(model->data(db_index, DbPasswordRole).toString());
	testdb.setPort(3306);

	if (testdb.isValid() && testdb.open() && validate_dbstruct(db_index)) {
		QMessageBox::information(this, windowTitle(), tr("Database") + " " + model->data(db_index, Qt::DisplayRole).toString() + " " + tr("access success"), QMessageBox::Close, QMessageBox::Close);
		testdb.close();
	} else {
		QMessageBox::warning(this, windowTitle(), tr("Database") + " " + model->data(db_index, Qt::DisplayRole).toString() + " " + tr("access deny"), QMessageBox::Close, QMessageBox::Close);
	}

	QSqlDatabase::removeDatabase("testdb");
}

void DialogDatabase::databaseRebuild()
{
	// TODO exclude sql worker to private thread;
	QSqlDatabase tgtdb = QSqlDatabase::addDatabase("QMYSQL", "testdb");

	QModelIndex db_index = ui->treeViewDatabases->currentIndex();
	ui_rebuild->listWidget->clear();
	tgtdb.setDatabaseName(model->data(db_index, DbBaseRole).toString());
	tgtdb.setHostName(model->data(db_index.parent(), Qt::DisplayRole).toString());
	tgtdb.setUserName(model->data(db_index, DbUserRole).toString());
	tgtdb.setPassword(model->data(db_index, DbPasswordRole).toString());
	tgtdb.setPort(3306);

	if (tgtdb.isValid() && tgtdb.open()) {

		if (QMessageBox::warning(this, windowTitle(), tr("Rebuild database") + " " + model->data(db_index, Qt::DisplayRole).toString() + "?\n" + tr("WARNING! This operation remove all data in current database."), QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes) {
			setCursor(Qt::WaitCursor);
			ui_rebuild->pushButtonClose->setDisabled(true);
			dialog_rebuild->setModal(true);
			dialog_rebuild->setWindowFlags(Qt::Dialog | Qt::WindowMaximizeButtonHint );
			dialog_rebuild->show();
			ui_rebuild->listWidget->insertItem(ui_rebuild->listWidget->count() ,new QListWidgetItem(QIcon::fromTheme ("dialog-ok-apply") , tr("Database open success")));
			ui_rebuild->listWidget->update();

			for (int id = 0; id < tgtdb.tables(QSql::Tables).count(); id++) {
				QStringList tables = tgtdb.tables(QSql::Tables);
				while (!tables.isEmpty()) {
					QString tablename = tables.first();
					QSqlQuery query = QSqlQuery(tgtdb);
					query.exec("alter table " + tablename + "disable keys");
					query.exec("drop table if exists " + tablename);
					tables.pop_front();
				}
			}
			ui_rebuild->listWidget->insertItem(ui_rebuild->listWidget->count() ,new QListWidgetItem(QIcon::fromTheme("dialog-ok-apply"), tr("Delete old tables")));
			ui_rebuild->listWidget->update();

			QStringList sql_core = sqlQueryListFromFile();
			if (!sql_core.isEmpty()) {
				while (!sql_core.isEmpty()) {
					QString sqlitem = sql_core.first();
					QSqlQuery query = QSqlQuery(tgtdb);
					if (query.exec(sqlitem)) {
						ui_rebuild->listWidget->insertItem(ui_rebuild->listWidget->count() ,new QListWidgetItem(QIcon::fromTheme("dialog-ok-apply"), sqlitem.split("(").at(0)));
						ui_rebuild->listWidget->update();
					} else {
						ui_rebuild->listWidget->insertItem(ui_rebuild->listWidget->count() ,new QListWidgetItem(QIcon::fromTheme("dialog-error"), query.lastError().text()));
						ui_rebuild->listWidget->update();
						break;
					}
					sql_core.pop_front();
				}

			} else {
				ui_rebuild->listWidget->insertItem(ui_rebuild->listWidget->count() ,new QListWidgetItem(QIcon::fromTheme("dialog-error"), tr("Error load sql file")));
				ui_rebuild->listWidget->update();
			}

			ui_rebuild->pushButtonClose->setEnabled(true);
			setCursor(Qt::ArrowCursor);
			ui->treeViewDatabases->setCurrentIndex(model->item(model->rowCount()-1)->index());
			tgtdb.close();
		}
	} else {
		QMessageBox::warning(this, windowTitle(), tr("Database") + " " + model->data(db_index, Qt::DisplayRole).toString() + " " + tr("access deny"), QMessageBox::Close, QMessageBox::Close);
	}
	QSqlDatabase::removeDatabase("tgtdb");
}

void DialogDatabase::validate_hostname(QString hostname)
{
	// TODO validate for URL or IP correct
	bool is_not_unique = false;
	for (int hostid = 0; hostid < model->rowCount(); hostid++) {
		if (hostname == model->item(hostid)->data(Qt::DisplayRole).toString()) is_not_unique = true;
	}
	ui_hostname->pushButtonCreate->setDisabled((hostname.length()<3) | is_not_unique);
}

void DialogDatabase::validate_database()
{
	bool is_complite = !ui_database->lineEditDatabase->text().isEmpty() & !ui_database->lineEditUserName->text().isEmpty();
	bool is_unique = true;

	for (int baseid = 0; baseid < model->item(ui->treeViewDatabases->currentIndex().row())->rowCount(); baseid++) {
		if (ui_database->lineEditUserName->text()+"@"+ui_database->lineEditDatabase->text() == model->item(ui->treeViewDatabases->currentIndex().row())->child(baseid)->text()) is_unique = false;
	}
	// TODO validate to correct username and dbase name
	ui_database->pushButtonCreate->setEnabled(is_complite & is_unique);
}

QStringList DialogDatabase::sqlQueryListFromFile()
{
	QStringList list;
	QString to_commit;
	QFile file(DialogConfigure::cfg()->value("SQL_Files", "dbcore", "/usr/share/cwall/sql/schema.mysql.sql").toString());
	if (file.open(QIODevice::ReadOnly)) {
		while (!file.atEnd()) {
			QString tempstr = QString::fromLocal8Bit(file.readLine()).simplified();
			if (!tempstr.isEmpty() & (tempstr.left(2)!="--")) {
				to_commit += " " + tempstr;
				if (tempstr.right(1)==";") {
					list << to_commit;
					to_commit.clear();
				}
			}
		}
		file.close();
	}

	return list;
}

bool DialogDatabase::validate_dbstruct(QModelIndex index)
{
	bool ret = true;

	QString host, base, user, passwd;
	int port;

	if (index.isValid()) {
		host = model->data(index.parent(), Qt::DisplayRole).toString();
		base = model->data(index, DbBaseRole).toString();
		user = model->data(index, DbUserRole).toString();
		passwd = model->data(index, DbPasswordRole).toString();
		port = 3306;
	} else {
		if (!DialogConfigure::cfg()->defaultDatabase(&host, &base, &user, &passwd, &port)) return false;
	}

	QSqlDatabase probedb = QSqlDatabase::addDatabase("QMYSQL", "cwall_probe");
	if (probedb.open()) {
		QStringList tables = probedb.tables(QSql::Tables);
		// TODO Add test for database structure complit (check table list (tables) to exists requred tables)

		probedb.close();
	} else {
		ret = false;
	}

	QSqlDatabase::removeDatabase("cwall_probe");
	return ret;
}


#include "dialogdatabase.moc"
