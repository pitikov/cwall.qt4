#include <QDebug>

#include "formlibraryeditor.h"
#include "dialogconfigure.h"
#include "user.h"

FormLibraryEditor::FormLibraryEditor(QWidget* parent, Qt::WindowFlags flags)
	: QWidget(parent, flags)
	, ui(new Ui::FormLibraryEditor)
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

	modelDiffucalty->setTable("lib_climbdifficalty");
	ui->listViewDiffucalties->setModel(modelDiffucalty);
	ui->listViewDiffucalties->setModelColumn(modelDiffucalty->fieldIndex("label"));
	modelDiffucalty->sort(modelDiffucalty->fieldIndex("id"), Qt::AscendingOrder);
	modelDiffucalty->select();

	modelRanges->setTable("lib_sportrange");
	ui->listViewSportRanges->setModel(modelRanges);
	ui->listViewSportRanges->setModelColumn(modelRanges->fieldIndex("label"));
	modelRanges->sort(modelRanges->fieldIndex("id"), Qt::AscendingOrder);
	modelRanges->select();

	modelCompetitionTypes->setTable("lib_competitiontype");
	ui->tableViewCompetitionTypes->setModel(modelCompetitionTypes);
	modelCompetitionTypes->select();
	ui->tableViewCompetitionTypes->hideColumn(modelCompetitionTypes->fieldIndex("prefix"));
	ui->tableViewCompetitionTypes->hideColumn(modelCompetitionTypes->fieldIndex("id"));

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
	modelCompetitors->select();

	modelUser->setTable("site_user");
	ui->tableViewUsers->setModel(modelUser);
	ui->tableViewUsers->hideColumn(modelUser->fieldIndex("pwdhash"));
	modelUser->sort(modelUser->fieldIndex("uid"), Qt::AscendingOrder);
	modelCompetitors->setRelation(modelCompetitors->fieldIndex("team"),QSqlRelation("lib_team", "gid", "team"));
	modelUser->select();

	ui->listWidgetCatalog->setCurrentItem(0);
	ui->stackedWidget->setCurrentIndex(0);

	if (!User::current()->isSuperuser()) {
		ui->listWidgetCatalog->model()->removeRow(ui->listWidgetCatalog->count()-1);
	}
}

FormLibraryEditor::~FormLibraryEditor()
{

}

#include "formlibraryeditor.moc"
