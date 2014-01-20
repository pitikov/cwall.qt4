#ifndef FORMLIBRARYEDITOR_H
#define FORMLIBRARYEDITOR_H

#include <QWidget>
#include <QSqlTableModel>
#include <QSqlRelationalTableModel>

#include "ui_form_library_editor.h"
#include "ui_dialog_user_password.h"

namespace Ui {
	class FormLibraryEditor;
	class DialogUserPassword;
}

class FormLibraryEditor : public QWidget
{
    Q_OBJECT
public:
	explicit FormLibraryEditor(QWidget* parent, Qt::WindowFlags flags = 0);
	~FormLibraryEditor();

private slots:
	void modelChange();
	void pageChanged( const int& page );
	void currentUserChanged(const QModelIndex &index );
	void userAdd();
	void userDataValidate(const QString &arg);
	void userPasswdChange();
	void modelUserCommit();
	void modelUserUpdate();
	void currentUserDelete();
	void modelDifficultyUpdate();
	void modelDifficultyCommit();
	void currentDiffChanged(const QModelIndex &index);
	void diffAdd();
	void diffDelete();
	void competitionTypeCommit();
	void competitionTypeUpdate();
	void competitorFind(const QString &index);
	void competitorAdd();
	void competitorDelete();
	void competitorMask();
	void competitorCommit();
	void competitorUpdate();
	void teamAdd();
	void teamDelete();
	void teamUpdate();
	void teamCommit();
	void currentTeamChanged(const QModelIndex &index);
	void agrpAdd();
	void agrpDelete();
	void agrpCommit();
	void agrpUpdate();
	void currentAgrpChanged(const QModelIndex &index);

private:
	Ui::FormLibraryEditor *ui;
	Ui::DialogUserPassword *ui_user_pwd;
	QSqlDatabase	cwall;
	QSqlTableModel *modelDiffucalty;
	QSqlTableModel *modelRanges;
	QSqlTableModel *modelCompetitionTypes;
	QSqlTableModel *modelAgeGroups;
	QSqlTableModel *modelTeams;
	QSqlRelationalTableModel *modelCompetitors;
	QSqlTableModel *modelUser;
};

#endif // FORMLIBRARYEDITOR_H
