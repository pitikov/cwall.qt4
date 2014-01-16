#ifndef RULESVIEWER_H
#define RULESVIEWER_H

#include <QWidget>
#include <QGraphicsScene>
#include <ui_form_rules_viewer.h>
#include <poppler/qt4/poppler-qt4.h>

namespace Ui {
	class FormRulesViewer;
}
class FormRulesViewer : public QWidget
{
    Q_OBJECT
public:
	FormRulesViewer(QWidget* parent, Qt::WindowFlags flags = 0);
	virtual ~FormRulesViewer();

private slots:
	void setPage(const int &page);
	void pageResize(const int &page);
	void textSearch(const QString& text, Poppler::Page::SearchDirection direct = Poppler::Page::NextResult);
	
private:
	Ui::FormRulesViewer *ui;
	Poppler::Document *doc;
	QGraphicsScene *scene;
	int current_page;
	bool key_ctrl_active;
	bool key_alt_active;
	bool eventFilter(QObject *sender, QEvent *event);
};

#endif // RULESVIEWER_H
