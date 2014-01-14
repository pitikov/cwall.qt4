#include <QDebug>
#include <QSqlRecord>
#include <QCryptographicHash>

#include "formlibraryeditor.h"
#include "dialogconfigure.h"
#include "user.h"

FormLibraryEditor::FormLibraryEditor(QWidget* parent, Qt::WindowFlags flags)
	: QWidget(parent, flags)
	, ui(new Ui::FormLibraryEditor)
	, ui_user_pwd(new Ui::DialogUserPassword)
	, cwall(QSqlDatabase::database("cwall"))
	, modelDiffucalty(new QSqlTableModel(this, cwall))
	, modelRanges(new QSqlTableModel(this, cwall))
	, modelCompetitionTypes(new QSqlTableModel(this, cwall))
	, modelAgeGroups(new QSqlTableModel(this, cwall))
	, modelTeams(new QSqlTableModel(this, cwall))
	, modelCompetitors(new QSqlRelationalTableModel(this, cwall))
	, modelUser(new QSqlTableModel(this, cwall))
{
	ui->setupUi(this);

	QAction *actionSeparator = new QAction(this);
	actionSeparator->setSeparator(true);
	QAction *actionSeparator1 = new QAction(this);
	actionSeparator1->setSeparator(true);

	modelDiffucalty->setTable("lib_climbdifficalty");
	ui->listViewDiffucalties->setModel(modelDiffucalty);
	ui->listViewDiffucalties->setModelColumn(modelDiffucalty->fieldIndex("label"));
	modelDiffucalty->sort(modelDiffucalty->fieldIndex("id"), Qt::AscendingOrder);
	modelDiffucalty->select();
	modelDiffucalty->setEditStrategy(QSqlTableModel::OnManualSubmit);
	ui->actionDifficultyCommit->setDisabled(true);
	if (User::current()->isSuperuser()) {
		ui->listViewDiffucalties->setContextMenuPolicy(Qt::ActionsContextMenu);
		ui->listViewDiffucalties->addAction(ui->actionDifficultyUpdate);
		ui->listViewDiffucalties->addAction(ui->actionDifficultyCommit);
		ui->listViewDiffucalties->addAction(actionSeparator);
		ui->listViewDiffucalties->addAction(ui->actionDifficultyAdd);
		ui->listViewDiffucalties->addAction(ui->actionDifficultyDelete);
		connect(ui->listViewDiffucalties, SIGNAL(pressed(QModelIndex)), this, SLOT(currentDiffChanged(QModelIndex)));
		connect(ui->listViewDiffucalties, SIGNAL(activated(QModelIndex)), this, SLOT(currentDiffChanged(QModelIndex)));
		connect(modelDiffucalty, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(modelChange()));
	} else {
		ui->listViewDiffucalties->setEditTriggers(QAbstractItemView::NoEditTriggers);
	}

	modelRanges->setTable("lib_sportrange");
	ui->listViewSportRanges->setModel(modelRanges);
	ui->listViewSportRanges->setModelColumn(modelRanges->fieldIndex("label"));
	modelRanges->sort(modelRanges->fieldIndex("id"), Qt::AscendingOrder);
	modelRanges->select();
	ui->listViewSportRanges->setEditTriggers(QAbstractItemView::NoEditTriggers);

	modelCompetitionTypes->setTable("lib_competitiontype");
	ui->tableViewCompetitionTypes->setModel(modelCompetitionTypes);
	modelCompetitionTypes->select();
	ui->tableViewCompetitionTypes->hideColumn(modelCompetitionTypes->fieldIndex("prefix"));
	ui->tableViewCompetitionTypes->hideColumn(modelCompetitionTypes->fieldIndex("id"));
	modelCompetitionTypes->setEditStrategy(QSqlTableModel::OnManualSubmit);
	if (User::current()->isSuperuser()) {
		ui->tableViewCompetitionTypes->addAction(ui->actionCTypeCommit);
		ui->tableViewCompetitionTypes->addAction(ui->actionCTypeUpdate);
		ui->actionCTypeCommit->setDisabled(true);
		connect(modelCompetitionTypes, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(modelChange()));
	} else {
		ui->tableViewCompetitionTypes->setEditTriggers(QAbstractItemView::NoEditTriggers);
	}

	modelAgeGroups->setTable("lib_agegroup");
	ui->tableViewAgeGroups->setModel(modelAgeGroups);
	modelAgeGroups->select();
	ui->tableViewAgeGroups->hideColumn(modelAgeGroups->fieldIndex("id"));
	modelAgeGroups->sort(modelAgeGroups->fieldIndex("id"), Qt::AscendingOrder);

	modelTeams->setTable("lib_team");
	ui->tableViewTeams->setModel(modelTeams);
	ui->tableViewTeams->hideColumn(modelTeams->fieldIndex("gid"));
	modelTeams->sort(modelTeams->fieldIndex("gid"), Qt::AscendingOrder);
	modelTeams->select();

	modelCompetitors->setTable("lib_competitor");
	ui->tableViewCompetitors->setModel(modelCompetitors);
	ui->tableViewCompetitors->hideColumn(modelCompetitors->fieldIndex("uid"));
	modelCompetitors->setRelation(modelCompetitors->fieldIndex("team"),QSqlRelation("lib_team", "gid", "team"));
	modelCompetitors->select();

	modelUser->setTable("site_user");
	ui->tableViewUsers->setModel(modelUser);
	ui->tableViewUsers->hideColumn(modelUser->fieldIndex("pwdhash"));
	ui->tableViewUsers->hideColumn(modelUser->fieldIndex("uid"));
	modelUser->sort(modelUser->fieldIndex("uid"), Qt::AscendingOrder);
	modelUser->select();
	ui->tableViewUsers->addAction(ui->actionUserUpdate);
	ui->tableViewUsers->addAction(ui->actionUserCommit);

	ui->tableViewUsers->addAction(actionSeparator);
	ui->tableViewUsers->addAction(ui->actionUserChangePasswd);
	ui->tableViewUsers->addAction(ui->actionUserDelete);
	modelUser->setEditStrategy(QSqlTableModel::OnManualSubmit);
	currentUserChanged(QModelIndex());
	ui->pushButtonUserCommit->setDisabled(true);

	ui->listWidgetCatalog->setCurrentItem(0);
	ui->stackedWidget->setCurrentIndex(0);

	if (!User::current()->isSuperuser()) {
		ui->listWidgetCatalog->model()->removeRow(ui->listWidgetCatalog->count()-1);
	}

	connect(modelUser, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(modelChange()));
}

FormLibraryEditor::~FormLibraryEditor()
{

}

void FormLibraryEditor::currentUserChanged(const QModelIndex& index)
{
	if (index.isValid()) {
		ui->actionUserUpdate->setEnabled(true);
		ui->actionUserChangePasswd->setEnabled(true);
		ui->actionUserDelete->setEnabled(modelUser->record(index.row()).value("uid").toInt()!=1);
	} else {
		ui->actionUserChangePasswd->setDisabled(true);
		ui->actionUserCommit->setDisabled(true);
		ui->actionUserDelete->setDisabled(true);
		ui->actionUserUpdate->setDisabled(true);
	}
}

void FormLibraryEditor::userAdd()
{
	int row = modelUser->rowCount();
	if (modelUser->insertRecord(-1, QSqlRecord())) {
		ui->actionUserCommit->setEnabled(true);

		modelUser->setData(modelUser->index(row, modelUser->fieldIndex("login")), ui->lineEditUserLogin->text().simplified());
		modelUser->setData(modelUser->index(row, modelUser->fieldIndex("name")), ui->lineEditUserName->text());
		modelUser->setData(modelUser->index(row, modelUser->fieldIndex("pwdhash")), QCryptographicHash::hash(ui->lineEditUserPasswd->text().toLatin1(), QCryptographicHash::Md5).toHex());
		ui->tableViewUsers->update();

		ui->lineEditUserLogin->clear();
		ui->lineEditUserName->clear();
		ui->lineEditUserPasswd->clear();
		ui->pushButtonUserCommit->setDisabled(true);
	}
}

void FormLibraryEditor::userDataValidate(const QString& arg)
{
	bool pwd_len = ui->lineEditUserPasswd->text().length() >= 8;;
	bool name_len = ui->lineEditUserName->text().length() >= 8;
	bool login_len = ui->lineEditUserLogin->text().length() >= 4;

	bool unq_login = true;
	for (int id = 0; id < modelUser->rowCount(); id++) {
		if (modelUser->record(id).value("login").toString() == ui->lineEditUserLogin->text().simplified()) {
			unq_login = false;
			break;
		}
	}
	ui->pushButtonUserCommit->setEnabled(pwd_len & name_len & login_len & unq_login);
}

void FormLibraryEditor::currentUserDelete()
{
	modelUser->removeRow(ui->tableViewUsers->currentIndex().row());
	currentUserChanged(QModelIndex());
	ui->actionUserCommit->setEnabled(true);
}

void FormLibraryEditor::modelUserCommit()
{
	if (modelUser->submitAll()) {
		modelUser->select();
		currentUserChanged(QModelIndex());
		ui->tableViewUsers->update();
		ui->actionUserCommit->setDisabled(true);
	}
}

void FormLibraryEditor::modelUserUpdate()
{
	modelUser->revertAll();
	modelUser->select();
	ui->tableViewUsers->update();
	currentUserChanged(QModelIndex());
	ui->actionUserCommit->setDisabled(true);
}


void FormLibraryEditor::userPasswdChange()
{
	QDialog dialog_pwd_change(this);
	ui_user_pwd->setupUi(&dialog_pwd_change);
	dialog_pwd_change.setModal(true);

	ui_user_pwd->lineEditPasswd->clear();
	ui_user_pwd->lineEditUser->setText(modelUser->record(ui->tableViewUsers->currentIndex().row()).value("name").toString());

	if (dialog_pwd_change.exec()==QDialog::Accepted) {
		modelUser->setData(modelUser->index(ui->tableViewUsers->currentIndex().row(), modelUser->fieldIndex("pwdhash")),
											 QCryptographicHash::hash(ui_user_pwd->lineEditPasswd->text().toLatin1(), QCryptographicHash::Md5).toHex());
	}
}

void FormLibraryEditor::modelChange()
{
	if (sender()==modelUser) ui->actionUserCommit->setEnabled(true);
	if (sender()==modelDiffucalty) ui->actionDifficultyCommit->setEnabled(true);
	if (sender()==modelCompetitionTypes) ui->actionCTypeCommit->setEnabled(true);
}

void FormLibraryEditor::modelDifficultyCommit()
{
	if (modelDiffucalty->submitAll()) {
		ui->listViewDiffucalties->update();
		ui->actionDifficultyCommit->setDisabled(true);
	}
}

void FormLibraryEditor::modelDifficultyUpdate()
{
	modelDiffucalty->revertAll();
	ui->listViewDiffucalties->update();
	ui->actionDifficultyCommit->setDisabled(true);
}

void FormLibraryEditor::currentDiffChanged(const QModelIndex& index)
{
	ui->actionDifficultyDelete->setEnabled(index.isValid());
}

void FormLibraryEditor::diffAdd()
{
	int row = ui->listViewDiffucalties->currentIndex().isValid()?ui->listViewDiffucalties->currentIndex().row():-1;
	modelDiffucalty->insertRecord(row, QSqlRecord());
}

void FormLibraryEditor::diffDelete()
{
	if (ui->listViewDiffucalties->currentIndex().isValid()) {
		modelDiffucalty->removeRow(ui->listViewDiffucalties->currentIndex().row());
	}
}

void FormLibraryEditor::competitionTypeCommit()
{
	if (modelCompetitionTypes->submitAll()) {
		ui->tableViewCompetitionTypes->update();
		ui->actionCTypeCommit->setDisabled(true);
	}
}

void FormLibraryEditor::competitionTypeUpdate()
{
	modelCompetitionTypes->revertAll();
	ui->tableViewCompetitionTypes->update();
	ui->actionCTypeCommit->setDisabled(true);
}


#include "formlibraryeditor.moc"
