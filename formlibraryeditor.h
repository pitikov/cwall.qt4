#ifndef FORMLIBRARYEDITOR_H
#define FORMLIBRARYEDITOR_H

#include <QWidget>

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
// 	void setShown(bool);

private:
	Ui::FormLibraryEditor *ui;
};

#endif // FORMLIBRARYEDITOR_H
