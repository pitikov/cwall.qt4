#include "dialogconfigure.h"
#include "rulesviewer.h"

#include <QDebug>
#include <QGraphicsItem>
#include <QCursor>
#include <QKeyEvent>

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

	setPage(current_page);

	ui->spinBoxCurrentPage->setMaximum(doc->numPages()-1);
	ui->lineEditPageLimit->setText(QString::number(doc->numPages()));

	setWindowTitle(tr("Competition rules"));

	connect(ui->spinBoxCurrentPage, SIGNAL(valueChanged(int)), SLOT(setPage(int)));
	ui->graphicsView->installEventFilter(this);
	qDebug() << doc->metadata();
}

FormRulesViewer::~FormRulesViewer()
{
}

void FormRulesViewer::setPage(const int& page)
{
	current_page = page-1;
	scene->clear();
	delete scene;
	scene = new QGraphicsScene(this);
	//scene->setSceneRect(-400,-600,0,0);
	ui->graphicsView->setScene(scene);
	scene->setBackgroundBrush(QBrush(Qt::gray ,Qt::SolidPattern));
	
	int scale_x = (ui->spinBoxPercent->value() * physicalDpiX())/100;
	int scale_y = (ui->spinBoxPercent->value() * physicalDpiY())/100;
	scene->addPixmap(QPixmap::fromImage(doc->page(current_page)->renderToImage(scale_x, scale_y)));
}

void FormRulesViewer::pageResize(const int& page)
{
	setPage(current_page+1);
}

void FormRulesViewer::textSearch(const QString& text, Poppler::Page::SearchDirection direct)
{
	if (!text.isEmpty()) {
		int page = current_page;
		while (page < doc->numPages()) {
			QRectF localtion;
	/*		if (doc->page(page)->search(text, localtion, Poppler::Page::NextResult, Poppler::Page::CaseInsensitive)) {
				
			}*/
			page++;
		}
		// TODO QMessageBox::question for search from start
		
	}
}

bool FormRulesViewer::eventFilter(QObject* sender, QEvent* event)
{
	bool ret = true;
	bool is_recived = false;
	if (sender == ui->graphicsView) {
		switch (event->type()) {
			case QEvent::Wheel :
				if (key_ctrl_active) {
					int delta = ((QWheelEvent*)event)->delta()>0?10:-10;
					int scale = ui->spinBoxPercent->value() + delta;
					ui->spinBoxPercent->setValue(scale);
					is_recived = true;
				} else if (key_alt_active) {
					
				} else {
					qDebug();
				}
				event->ignore();
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
				qDebug() << event->type();
				break;
		}
	}
	if (!is_recived) ret = QObject::eventFilter(sender, event);
	return ret;
}


#include "rulesviewer.moc"
