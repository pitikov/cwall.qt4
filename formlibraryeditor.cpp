#include "formlibraryeditor.h"

FormLibraryEditor::FormLibraryEditor(QWidget* parent, Qt::WindowFlags flags)
	: QWidget(parent, flags)
	, ui(new Ui::FormLibraryEditor)
{
	ui->setupUi(this);
}

FormLibraryEditor::~FormLibraryEditor()
{

}

// void FormLibraryEditor::setShown(bool is_shown)
// {
// 	if (sender()->objectName() == "actionLibrary_static_data") {
// 		QAction *action = (QAction*)sender();
// 		action->setChecked(is_shown);
// 	}
// 	//parentWidget()->setShown(is_shown);
// 	//if (is_shown) parentWidget()->show(); else parentWidget()->hide();
// 	QWidget::setShown(is_shown);
// }

#include "formlibraryeditor.moc"
