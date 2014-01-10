#ifndef DIALOGDATABASE_H
#define DIALOGDATABASE_H

#include <QSqlDatabase>
#include <QDialog>
#include <QAction>
#include <QIcon>
#include <QStandardItemModel>

#include "ui_dialog_database.h"
#include "ui_dialog_hostname.h"
#include "ui_dialog_db_create.h"
#include "ui_dialog_rebuild.h"

namespace Ui {
	class DialogDatabase;
	class DialogHostname;
	class DialogDbCreate;
	class DialogRebuild;
}

class DialogDatabase : public QDialog
{
	Q_OBJECT
public:
	explicit DialogDatabase(QWidget *parent);
	virtual ~DialogDatabase();

	void accept();
	void reject();
	bool validate_dbstruct( QModelIndex index = QModelIndex() );

private:

	Ui::DialogDatabase	*ui;
	Ui::DialogHostname	*ui_hostname;
	Ui::DialogDbCreate	*ui_database;
	Ui::DialogRebuild		*ui_rebuild;
	QDialog				*dialog_hostname;
	QDialog				*dialog_database;
	QDialog				*dialog_rebuild;
	QStandardItemModel	*model;
	int					DbPasswordRole;
	int					DbDefaultRole;
	int					DbBaseRole;
	int					DbUserRole;

	QAction	*actionHostAdd;
	QAction	*actionHostDelete;
	QAction	*actionDbAdd;
	QAction	*actionDbDelete;
	QAction	*actionDbSetDefault;
	QAction	*actionDbTest;
	QAction	*actionDbRebuild;

	QSqlDatabase dbase;

	QStringList sqlQueryListFromFile();

private slots:
	int exec();
	void indexChanged(QModelIndex index);
	void hostnameAdd();
	void hostnameDelete();
	void databaseAdd();
	void databaseDelete();
	void databaseSetDefault(QModelIndex index = QModelIndex());
	void databaseTest();
	void databaseRebuild();

	void validate_hostname(QString hostname);
	void validate_database();
};

#endif // DIALOGDATABASE_H
