#include <QDebug>
#include <QSqlRecord>
#include <QSqlRelationalDelegate>
#include <QCryptographicHash>
#include <QtSql>

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

	setMinimumWidth(600);
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
	if (User::current()->isSuperuser() & DialogConfigure::cfg()->registration()) {
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
	if (User::current()->isSuperuser() & DialogConfigure::cfg()->registration()) {
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
	modelAgeGroups->setEditStrategy(QSqlTableModel::OnManualSubmit);
	if (DialogConfigure::cfg()->registration()) {
		ui->tableViewAgeGroups->addAction(ui->actionAgeGroupUpdate);
		ui->tableViewAgeGroups->addAction(ui->actionAgeGroupCommit);
		ui->tableViewAgeGroups->addAction(actionSeparator);
		ui->tableViewAgeGroups->addAction(ui->actionAgeGroupAdd);
		ui->tableViewAgeGroups->addAction(ui->actionAgeGroupDelete);
		ui->actionAgeGroupCommit->setDisabled(true);
		ui->actionAgeGroupDelete->setDisabled(true);
		connect(modelAgeGroups, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(modelChange()));
		connect(ui->tableViewAgeGroups, SIGNAL(activated(QModelIndex)), this, SLOT(currentAgrpChanged(QModelIndex)));
		connect(ui->tableViewAgeGroups, SIGNAL(pressed(QModelIndex)), this, SLOT(currentAgrpChanged(QModelIndex)));
	} else {
		ui->tableViewAgeGroups->setEditTriggers(QAbstractItemView::NoEditTriggers);
	}

	modelTeams->setTable("lib_team");
	ui->tableViewTeams->setModel(modelTeams);
	ui->tableViewTeams->hideColumn(modelTeams->fieldIndex("gid"));
	modelTeams->sort(modelTeams->fieldIndex("gid"), Qt::AscendingOrder);
	modelTeams->select();
	ui->tableViewTeams->addAction(ui->actionTeamCommit);
	ui->tableViewTeams->addAction(ui->actionTeamUpdate);
	ui->tableViewTeams->addAction(actionSeparator);
	ui->tableViewTeams->addAction(ui->actionTeamAdd);
	ui->tableViewTeams->addAction(ui->actionTeamDelete);
	modelTeams->setEditStrategy(QSqlTableModel::OnManualSubmit);
	ui->actionTeamCommit->setDisabled(true);
	ui->actionTeamDelete->setDisabled(true);
	connect(modelTeams, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(modelChange()));
	connect(ui->tableViewTeams, SIGNAL(activated(QModelIndex)), this, SLOT(currentTeamChanged(QModelIndex)));
	connect(ui->tableViewTeams, SIGNAL(pressed(QModelIndex)), this, SLOT(currentTeamChanged(QModelIndex)));

	modelCompetitors->setTable("lib_competitor");
	ui->tableViewCompetitors->setModel(modelCompetitors);
	ui->tableViewCompetitors->hideColumn(modelCompetitors->fieldIndex("uid"));
	modelCompetitors->setRelation(modelCompetitors->fieldIndex("team"),QSqlRelation("lib_team", "gid", "title"));
	modelCompetitors->setRelation(modelCompetitors->fieldIndex("range"),QSqlRelation("lib_sportrange", "id", "label"));
	modelCompetitors->select();
	ui->comboBoxCompetitorGender->addItem(tr("Male"), "М");
	ui->comboBoxCompetitorGender->addItem(tr("Female"), "Ж");
	for (int id = 0; id < modelRanges->rowCount(); id++) {
		ui->comboBoxCompetitorRange->addItem(modelRanges->record(id).value("label").toString(), modelRanges->record(id).value("id").toInt());
	}
	for (int id = 0; id < modelTeams->rowCount(); id++) {
		ui->comboBoxCompetitorTeam->addItem(modelTeams->record(id).value("title").toString() + " (" + modelTeams->record(id).value("region").toString() + ")", modelTeams->record(id).value("gid").toInt());
	}
	modelCompetitors->setEditStrategy(QSqlTableModel::OnManualSubmit);

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
	ui->tableViewUsers->update();
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
	if (sender()==modelTeams) ui->actionTeamCommit->setEnabled(true);
	if (sender()==modelAgeGroups) ui->actionAgeGroupCommit->setEnabled(true);
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
	ui->actionDifficultyCommit->setEnabled(true);
	ui->listViewDiffucalties->update();
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

void FormLibraryEditor::competitorAdd()
{
	int row = modelCompetitors->rowCount();
	QSqlRecord new_competitor;

	QSqlField f_uid("uid");
	QSqlField f_name("name");
	QSqlField f_dob("dob");
	QSqlField f_gender("gender");
	QSqlField f_range("range");
	QSqlField f_team("team");
	f_uid.setAutoValue(true);
	f_name.setValue(ui->lineEditCompetitorName->text());
	f_gender.setValue(ui->comboBoxCompetitorGender->currentIndex()+1);
	f_range.setValue(modelRanges->record(ui->comboBoxCompetitorRange->currentIndex()).value("id"));
	f_team.setValue(modelTeams->record(ui->comboBoxCompetitorTeam->currentIndex()).value("gid"));

	new_competitor.append(f_uid);
	new_competitor.append(f_name);
	new_competitor.append(f_dob);
	new_competitor.append(f_gender);
	new_competitor.append(f_range);
	new_competitor.append(f_team);
	// FIXME Not inserting record to competitor list model
	if (modelCompetitors->insertRecord(-1, new_competitor)) {
	} else qDebug() << "Record inserting error";
}

void FormLibraryEditor::competitorFind(const QString& index)
{

}

void FormLibraryEditor::competitorDel()
{

}

void FormLibraryEditor::competitorMask()
{

}

void FormLibraryEditor::teamAdd()
{
	int row = modelTeams->rowCount();
	modelTeams->insertRecord(-1, QSqlRecord());
	ui->tableViewTeams->setCurrentIndex(modelTeams->index(row, modelTeams->fieldIndex("label")));
	ui->actionTeamCommit->setEnabled(true);
	ui->actionDifficultyDelete->setDisabled(true);
}

void FormLibraryEditor::teamCommit()
{
	modelTeams->submitAll();
	ui->actionTeamCommit->setDisabled(true);
	ui->tableViewTeams->update();
}

void FormLibraryEditor::teamDelete()
{
	if (ui->tableViewTeams->currentIndex().isValid()) modelTeams->removeRow(ui->tableViewTeams->currentIndex().row());
	ui->tableViewTeams->update();
	ui->actionTeamCommit->setEnabled(true);
}

void FormLibraryEditor::teamUpdate()
{
	modelTeams->revertAll();
	ui->actionTeamCommit->setDisabled(true);
	ui->tableViewTeams->update();
}

void FormLibraryEditor::currentTeamChanged(const QModelIndex& index)
{
	ui->actionTeamDelete->setEnabled(index.isValid());
}

void FormLibraryEditor::agrpAdd()
{
	if (modelAgeGroups->insertRecord(-1, QSqlRecord())) {
		ui->tableViewAgeGroups->update();
		ui->actionAgeGroupCommit->setEnabled(true);
	}
}

void FormLibraryEditor::agrpCommit()
{
	if (modelAgeGroups->submitAll())
	ui->actionAgeGroupCommit->setDisabled(true);
	ui->actionAgeGroupDelete->setDisabled(true);
	ui->tableViewAgeGroups->update();
}

void FormLibraryEditor::agrpDelete()
{
	if (ui->tableViewAgeGroups->currentIndex().isValid()) {
		modelAgeGroups->removeRow(ui->tableViewAgeGroups->currentIndex().row());
	}
	ui->tableViewAgeGroups->update();
	ui->actionAgeGroupCommit->setEnabled(true);
}

void FormLibraryEditor::agrpUpdate()
{
	modelAgeGroups->revertAll();
	ui->tableViewAgeGroups->update();
	ui->actionAgeGroupCommit->setDisabled(true);
}

void FormLibraryEditor::currentAgrpChanged( const QModelIndex& index )
{
	ui->actionAgeGroupDelete->setEnabled(index.isValid());
}

void FormLibraryEditor::pageChanged( const int& page )
{
	if (page == ui->stackedWidget->indexOf(ui->pageCompetitonTypes)) {

	} else if (page == ui->stackedWidget->indexOf(ui->pageClimbingDifficulty)) {
		modelDiffucalty->sort(modelDiffucalty->fieldIndex("label"), Qt::AscendingOrder);
	} else if (page == ui->stackedWidget->indexOf(ui->pageSportRanges)) {

	} else if (page == ui->stackedWidget->indexOf(ui->pageTeams)) {

	} else if (page ==  ui->stackedWidget->indexOf(ui->pageUsers)) {

	} else if (page ==  ui->stackedWidget->indexOf(ui->pageCompetitors)) {
		ui->comboBoxCompetitorTeam->clear();
		ui->comboBoxCompetitorRange->clear();
		for (int id = 0; id < modelRanges->rowCount(); id++) {
			ui->comboBoxCompetitorRange->addItem(modelRanges->record(id).value("label").toString(), modelRanges->record(id).value("id").toInt());
		}
		for (int id = 0; id < modelTeams->rowCount(); id++) {
			if (!modelTeams->record(id).value("title").toString().isEmpty()) ui->comboBoxCompetitorTeam->addItem(modelTeams->record(id).value("title").toString() + " (" + modelTeams->record(id).value("region").toString() + ")", modelTeams->record(id).value("gid").toInt());
		}
		ui->comboBoxCompetitorTeam->model()->sort(0, Qt::AscendingOrder);
		ui->comboBoxCompetitorTeam->adjustSize();
	} else if (page ==  ui->stackedWidget->indexOf(ui->pageAgeGroups)) {

	}
}

#include "formlibraryeditor.moc"
