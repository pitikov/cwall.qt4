#ifndef RULESVIEWER_H
#define RULESVIEWER_H

#include <QWidget>

#include <ui_form_rules_viewer.h>

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
	void pageResize(const int& scale);
	void findPrevious();
	void findNext();
	void startSearch();

private:
	Ui::FormRulesViewer *ui;
//	static int pX, pY;

	static int current_page;
	bool eventFilter(QObject *sender, QEvent *event);
	void pageNum(int pos_x, int pos_y);
};

#endif // RULESVIEWER_H
