#include "rules.h"
#include "dialogconfigure.h"
#include <QGraphicsItem>
#include <QPoint>
#include <QDebug>
#include <QDesktopWidget>

Rules *Rules::self_ = NULL;

Rules* Rules::sample(QObject* parent)
{
	if (!self_) self_ = new Rules( parent );
	return self_;
}

Rules::Rules( QObject *parent )
: QGraphicsScene( parent )
, doc_(Poppler::Document::load(DialogConfigure::cfg()->value("PDF_Files", "rules").toString()))
, list_pages(QList<QGraphicsItem*>())
, artefacts(QQueue<QGraphicsRectItem*>())
, searched_(QString())
, currentResult_(-1)
{
	setBackgroundBrush(QBrush(Qt::gray ,Qt::SolidPattern));
	int pos_y = 0;

	for (int id = 0; id < doc()->numPages(); id++) {
		// TODO need use real phisical device DPI

		QDesktopWidget *dstp = QApplication::desktop();

		QPixmap pixmap = QPixmap::fromImage(doc()->page(id)->renderToImage(dstp->physicalDpiX()*1.5, dstp->physicalDpiY()*1.5));
		QGraphicsItem *item = addPixmap(pixmap);
		list_pages.append(item);
		item->setPos( 20, pos_y );
		pos_y += (pixmap.height() + 20);
	}
}

Rules::~Rules()
{
	self_ = NULL;
}

QList< QGraphicsRectItem* > *Rules::search(const QString& str)
{
	// TODO In this moment use magic number 100 and 2. Fix this problemm
	qreal factorX = 100/QApplication::desktop()->physicalDpiX()*2;
	qreal factorY = 100/QApplication::desktop()->physicalDpiY()*2;
	searchClean();
	searched_ = str;
	artefacts.clear();
	for (int page=0; page < doc()->numPages(); page++) {
		QList<QRectF> list_words = doc()->page(page)->search(str, Poppler::Page::CaseInsensitive, Poppler::Page::Rotate0);

		for (int wid = 0; wid < list_words.count(); wid++) {
			QGraphicsRectItem *item = addRect(pager()->at(page)->pos().x() + list_words.at(wid).x()*factorX, pager()->at(page)->pos().y() + list_words.at(wid).y()*factorY, list_words.at(wid).width()*factorX, list_words.at(wid).height()*factorY, QPen(Qt::yellow), QBrush(Qt::yellow, Qt::Dense4Pattern));
			artefacts.enqueue(item);
		}
	}
	return &artefacts;
}

void Rules::searchClean()
{
	QGraphicsRectItem *item;
	while (!artefacts.isEmpty()) {
		removeItem(artefacts.dequeue());
	}
	searched_.clear();
	currentResult_ = -1;
	update();
}

QString Rules::searchText() const
{
	return searched_;
}

Poppler::Document* Rules::doc()
{
	return doc_;
}

QList< QGraphicsItem* >* Rules::pager()
{
	return &list_pages;
}

QQueue< QGraphicsRectItem* >* Rules::searched()
{
	return &artefacts;
}

int Rules::currentResult() const
{
	return currentResult_;
}

bool Rules::setCurrentResult(const int& number)
{
	if (number < 0) {
		if ((artefacts.count() > currentResult_) & (currentResult_ >=0 )) {
			artefacts.at(currentResult_)->setBrush(QBrush(Qt::yellow, Qt::Dense4Pattern));
			artefacts.at(currentResult_)->setPen(QPen(Qt::yellow));
			currentResult_ = -1;
		}
		return true;
	} else if ((number >= 0) & (number < artefacts.count())) {
		if (currentResult_ >= 0) {
			artefacts.at(currentResult_)->setBrush(QBrush(Qt::yellow, Qt::Dense4Pattern));
			artefacts.at(currentResult_)->setPen(QPen(Qt::yellow));
		}
		currentResult_ = number;
		artefacts.at(currentResult_)->setBrush(QBrush(Qt::red, Qt::Dense6Pattern));
		artefacts.at(currentResult_)->setPen(QPen(Qt::red));
		return true;
	} else return false;
}

#include "rules.moc"
