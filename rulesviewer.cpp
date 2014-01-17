#include "dialogconfigure.h"
#include "rulesviewer.h"

#include <QDebug>
#include <QGraphicsItem>
#include <QCursor>
#include <QKeyEvent>
#include <QPoint>
#include <QSize>
#include "rules.h"

int FormRulesViewer::current_page = 1;

FormRulesViewer::FormRulesViewer(QWidget* parent, Qt::WindowFlags flags )
	: QWidget(parent, flags)
	, ui(new Ui::FormRulesViewer)
{
	if (Rules::sample()->doc()) {
		close();
	}
	ui->setupUi(this);
	ui->graphicsView->setScene(Rules::sample());
	ui->lineEditPageLimit->setText(QString::number(Rules::sample()->doc()->numPages()));
	ui->spinBoxCurrentPage->setMaximum(Rules::sample()->doc()->numPages());

	setWindowTitle(tr("Competition rules"));

	connect(ui->spinBoxCurrentPage, SIGNAL(valueChanged(int)), SLOT(setPage(int)));

	ui->graphicsView->installEventFilter(this);
	ui->graphicsView->adjustSize();
	setPage(current_page);

	ui->lineEditFind->setText(Rules::sample()->searchText());
	if (Rules::sample()->searched()->isEmpty()) {
		ui->widgetItemsInfo->setHidden(true);
		ui->toolButtonNext->setDisabled(true);
		ui->toolButtonPrevious->setDisabled(true);
	} else {
		ui->toolButtonNext->setEnabled(true);
		ui->toolButtonPrevious->setEnabled(true);
		ui->widgetItemsInfo->setShown(true);
		ui->labelCountItem->setText(QString::number(Rules::sample()->searched()->count()));
		ui->labelCurrentItem->setText(QString::number(Rules::sample()->currentResult()+1));
	}
}

FormRulesViewer::~FormRulesViewer()
{
}

void FormRulesViewer::setPage(const int& page)
{
	if ((page > 0)&(page <= Rules::sample()->doc()->numPages())) {
		ui->graphicsView->centerOn(Rules::sample()->pager()->at(page-1));
		current_page = page-1;
	}
}

void FormRulesViewer::pageResize(const int& scale)
{
	ui->graphicsView->resetMatrix();

	ui->graphicsView->scale(((qreal)scale)/100, ((qreal)scale)/100);
}

bool FormRulesViewer::eventFilter(QObject* sender, QEvent* event)
{
	bool ret = true;
	bool is_recived = false;
	int pos_x, pos_y;
	if (sender == ui->graphicsView) {
		switch (event->type()) {
			case QEvent::Wheel :
				pos_x = ((QWheelEvent*)event)->x();
				pos_y = ((QWheelEvent*)event)->y();
				pageNum(pos_x, pos_y);
				event->ignore();
				break;
			case QEvent::HoverMove :
        pos_x = ((QHoverEvent*)event)->pos().x();
				pos_y = ((QHoverEvent*)event)->pos().y();
				pageNum(pos_x, pos_y);
				break;
			case QEvent::KeyPress :
				switch (((QKeyEvent*)event)->key()) {
					case Qt::Key_PageDown :
						ui->spinBoxCurrentPage->setValue(ui->spinBoxCurrentPage->value()+1);
						is_recived = true;
						event->accept();
						break;
					case Qt::Key_PageUp :
						ui->spinBoxCurrentPage->setValue(ui->spinBoxCurrentPage->value()-1);
						is_recived = true;
						event->accept();
						break;
					case Qt::Key_Home :
						ui->spinBoxCurrentPage->setValue(1);
						is_recived = true;
						event->accept();
						break;
					default:
						break;
				}
				break;
/*			case QEvent::KeyRelease :
				switch (((QKeyEvent*)event)->key()) {

					default:
						break;
				}*/
				break;
			default:
				//qDebug() << event->type();
				break;
		}
	}
	if (!is_recived) ret = QObject::eventFilter(sender, event);
	return ret;
}

void FormRulesViewer::findNext()
{
	int current;
	if (!Rules::sample()->searched()->isEmpty()) {
		if (Rules::sample()->currentResult() >= 0) current = Rules::sample()->currentResult()+1;
		if (Rules::sample()->currentResult() < 0) current = 0;
		if (current >= Rules::sample()->searched()->count()) current = 0;
		Rules::sample()->setCurrentResult(current);
		ui->graphicsView->centerOn(Rules::sample()->searched()->at(current));
		ui->labelCurrentItem->setText(QString::number(current+1));
	}
}

void FormRulesViewer::findPrevious()
{
	int current;
	if (!Rules::sample()->searched()->isEmpty()) {
		if (Rules::sample()->currentResult() >= 0) current = Rules::sample()->currentResult()-1;
		if (Rules::sample()->currentResult() < 0) current = Rules::sample()->searched()->count()-1;
		if (current < 0) current = Rules::sample()->searched()->count()-1;;
		Rules::sample()->setCurrentResult(current);
		ui->graphicsView->centerOn(Rules::sample()->searched()->at(current));
		ui->labelCurrentItem->setText(QString::number(current+1));
	}
}

void FormRulesViewer::startSearch()
{
	if (ui->lineEditFind->text().simplified().isEmpty()) {
		ui->widgetItemsInfo->setShown(false);
		Rules::sample()->searchClean();
	}
	else {
		ui->labelCountItem->setText(QString::number(Rules::sample()->search (ui->lineEditFind->text())->count()));

		if (!Rules::sample()->searched()->isEmpty()) {
			ui->widgetItemsInfo->setShown(true);
			ui->toolButtonNext->setEnabled(true);
			ui->toolButtonPrevious->setEnabled(true);

			// TODO get ferst result from bottom of the current point
			Rules::sample()->setCurrentResult(0);
			ui->graphicsView->centerOn(Rules::sample()->searched()->at(0));
			ui->labelCurrentItem->setText(QString::number(Rules::sample()->currentResult()+1));
		} else {
			ui->toolButtonNext->setDisabled(true);
			ui->toolButtonPrevious->setDisabled(true);
			ui->widgetItemsInfo->setShown(false);
		}
	}
}

void FormRulesViewer::pageNum(int pos_x, int pos_y)
{
	for (int id = 0; id < ui->graphicsView->items(pos_x, pos_y).count(); id++) {
		for (int j = 0; j < Rules::sample()->doc()->numPages(); j++) {
			if (ui->graphicsView->items(pos_x, pos_y).at(id) == Rules::sample()->pager()->at(j)) {
				current_page = j+1;
				ui->spinBoxCurrentPage->blockSignals(true);
				ui->spinBoxCurrentPage->setValue(current_page);
				ui->spinBoxCurrentPage->blockSignals(false);
			}
		}
	}
}

#include "rulesviewer.moc"
