#ifndef FORMLIBRARYEDITOR_H
#define FORMLIBRARYEDITOR_H

#include <QWidget>
#include <QSqlTableModel>

#include "ui_form_library_editor.h"

namespace Ui {
	class FormLibraryEditor;
}

class FormLibraryEditor : public QWidget
{
    Q_OBJECT
public:
	explicit FormLibraryEditor(QWidget* parent, Qt::WindowFlags flags = 0);
	~FormLibraryEditor();

public slots:

private:
	Ui::FormLibraryEditor *ui;
	QSqlDatabase	cwall;
	QSqlTableModel *modelDiffucalty;
	QSqlTableModel *modelRanges;
	QSqlTableModel *modelCompetitionTypes;
	QSqlTableModel *modelAgeGroups;
};

#endif // FORMLIBRARYEDITOR_H
