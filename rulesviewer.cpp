#include "dialogconfigure.h"
#include "rulesviewer.h"

#include <QDebug>
#include <QGraphicsItem>
#include <QCursor>

FormRulesViewer::FormRulesViewer(QWidget* parent, Qt::WindowFlags flags )
	: QWidget(parent, flags)
	, ui(new Ui::FormRulesViewer)
	, doc(Poppler::Document::load(DialogConfigure::cfg()->value("PDF_Files", "rules").toString()))
	, scene(new QGraphicsScene(this))
{
	setCursor(Qt::WaitCursor);
	if (!doc) {
		close();
	}
	ui->setupUi(this);
	ui->graphicsView->setScene(scene);

	scene->setBackgroundBrush(QBrush(Qt::Dense6Pattern));
	for (int page = 0; page < doc->numPages(); page++) {
		QPixmap pixmap = QPixmap::fromImage(doc->page(page)->renderToImage());
		scene->addPixmap(pixmap);

		//qDebug() << "Page" << page << ": " << pixmap.size();
		scene->items().at(page)->setPos(0, -page*( pixmap.size().height() + 20));
	}

	ui->spinBoxCurrentPage->setMaximum(doc->numPages()-1);
	ui->lineEditPageLimit->setText(QString::number(doc->numPages()));

	setWindowTitle(tr("Competition rules"));

	setCursor(Qt::ArrowCursor);
	connect(ui->spinBoxCurrentPage, SIGNAL(valueChanged(int)), SLOT(setPage(int)));
}

FormRulesViewer::~FormRulesViewer()
{
}

void FormRulesViewer::setPage(const int& page)
{
	scene->items().at(page)->show();
	qDebug() << page << "position" << scene->items().at(0)->pos().x();
}


#include "rulesviewer.moc"
