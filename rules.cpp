#include "rules.h"
#include "dialogconfigure.h"
#include <QGraphicsItem>
#include <QPoint>
#include <QDebug>

Rules *Rules::self_ = NULL;

Rules* Rules::sample(QObject* parent)
{
	if (!self_) self_ = new Rules( parent );
	return self_;
}

Rules::Rules( QObject *parent )
: QGraphicsScene( parent )
, doc_(Poppler::Document::load(DialogConfigure::cfg()->value("PDF_Files", "rules").toString()))
, artefacts(QQueue<QGraphicsRectItem*>())
, searched(QString())
{
	setBackgroundBrush(QBrush(Qt::gray ,Qt::SolidPattern));
	int pos_y = 0;

	for (int id = 0; id < doc()->numPages(); id++) {
		// TODO need use real phisical device DPI
		QPixmap pixmap = QPixmap::fromImage(doc()->page(id)->renderToImage(150, 150));
		QGraphicsItem *item = addPixmap(pixmap);
		item->setPos( 20, pos_y );
		pos_y += (pixmap.height() + 20);
	}
}

Rules::~Rules()
{
	self_ = NULL;
}

QList< QGraphicsRectItem* > Rules::search(const QString& str)
{
	qDebug() << "****" << __func__ << "start" << "****";
	searchClean();
	searched = str;
	for (int page=0; page < doc()->numPages(); page++) {
		qDebug() << "page no" << page;
		QPointF page_pos = items().at(doc()->numPages()-(page+1))->pos();
		qDebug() << "get page position";
		QList<QRectF>art_list = doc()->page(page)->search(str, Poppler::Page::CaseInsensitive, Poppler::Page::Rotate0);
		qDebug() << "finded" << art_list.count();
		for (int id = 0; id < art_list.count(); id++) {
			QRectF text = art_list.at(id);
			qDebug() << "text including" << id;
			artefacts.enqueue(addRect(text.x()+page_pos.x(), text.y()+page_pos.y(), text.width(), text.height(), QPen(Qt::green), QBrush(Qt::green, Qt::Dense4Pattern)));
			qDebug() << "adding success";
			qDebug() << "--------------";
		}
		qDebug() << "----- end of page ------";
	}
	qDebug() << "=========== end of book =============";
	qDebug() << "****"<< __func__ << "complit"<< "****";
	return artefacts;
}

void Rules::searchClean()
{
	qDebug() << "****" << __func__ << "start" << "****";
	QGraphicsRectItem *item;
	while (!artefacts.isEmpty()) {
		items().removeOne(artefacts.dequeue());
	}
	searched.clear();
	update();
	qDebug() << "****"<< __func__ << "complit"<< "****";
}

QString Rules::searchText() const
{
	return searched;
}

Poppler::Document* Rules::doc()
{
	return doc_;
}

#include "rules.moc"
