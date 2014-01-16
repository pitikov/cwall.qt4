#include "dialogconfigure.h"
#include "rulesviewer.h"

#include <QDebug>
#include <QGraphicsItem>
#include <QCursor>
#include <QKeyEvent>
#include <QPoint>

FormRulesViewer::FormRulesViewer(QWidget* parent, Qt::WindowFlags flags )
	: QWidget(parent, flags)
	, ui(new Ui::FormRulesViewer)
	, doc(Poppler::Document::load(DialogConfigure::cfg()->value("PDF_Files", "rules").toString()))
	, scene(new QGraphicsScene(this))
	, current_page(1)
	, key_ctrl_active(false)
	, key_alt_active(false)
{
	if (!doc) {
		close();
	}
	ui->setupUi(this);
	ui->graphicsView->setScene(scene);
	ui->lineEditPageLimit->setText(QString::number(doc->numPages()));
	ui->spinBoxCurrentPage->setMaximum(doc->numPages());

	setWindowTitle(tr("Competition rules"));

	connect(ui->spinBoxCurrentPage, SIGNAL(valueChanged(int)), SLOT(setPage(int)));
	
	scene->setBackgroundBrush(QBrush(Qt::gray ,Qt::SolidPattern));
	ui->graphicsView->installEventFilter(this);
	
	int pos_y = 0;
	
	for (int id = 0; id < doc->numPages(); id++) {
		QPixmap pixmap = QPixmap::fromImage(doc->page(id)->renderToImage(physicalDpiX(), physicalDpiY()));
		QGraphicsItem *item = scene->addPixmap(pixmap);
		item->setPos( 20, pos_y );
		pos_y += (pixmap.height() + 20);
		scene->update();
	}
}

FormRulesViewer::~FormRulesViewer()
{
}

void FormRulesViewer::setPage(const int& page)
{
	ui->graphicsView->centerOn(scene->items().at(doc->numPages() - page));
}

void FormRulesViewer::pageResize(const int& scale)
{
	qreal zoom = (qreal)scale/100;
	//ui->graphicsView->scale(zoom, zoom);
}

void FormRulesViewer::textSearch(const QString& text, Poppler::Page::SearchDirection direct)
{
	if (!text.isEmpty()) {
		int page = current_page;
		while (page < doc->numPages()) {
			QRectF localtion;

			page++;
		}
		// TODO QMessageBox::question for search from start
		
	}
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
					case Qt::Key_Alt :
						key_alt_active = true;
						break;
					default:
						break;
				}
				break;
			case QEvent::KeyRelease :
				switch (((QKeyEvent*)event)->key()) {
					case Qt::Key_Control :
						key_ctrl_active = false;
						return true;
						break;
					case Qt::Key_Alt :
						key_alt_active = false;
						break;
					default:
						break;
				}
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
	qWarning() << "Implict me please" << __func__;
}

void FormRulesViewer::findPrevious()
{
	qWarning() << "Implict me please" << __func__;
}

void FormRulesViewer::startSearch()
{
	qWarning() << "Implict me please" << __func__;
}

void FormRulesViewer::pageNum(int pos_x, int pos_y)
{
	for (int id = 0; id < ui->graphicsView->items(pos_x, pos_y).count(); id++) {
		for (int j = 0; j < scene->items().count(); j++) {
			if (ui->graphicsView->items(pos_x, pos_y).at(id) == scene->items().at(j)) {
				ui->spinBoxCurrentPage->blockSignals(true);
				ui->spinBoxCurrentPage->setValue(doc->numPages()-j);
				ui->spinBoxCurrentPage->blockSignals(false);
			}
		}
	}
}

#include "rulesviewer.moc"
