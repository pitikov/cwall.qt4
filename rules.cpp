#include "rules.h"
#include "dialogconfigure.h"
#include <QGraphicsItem>
#include <QPoint>

Rules *Rules::self_ = NULL;

Rules* Rules::sample(QObject* parent)
{
	if (!self_) self_ = new Rules( parent );
	return self_;
}

Rules::Rules( QObject *parent )
: QGraphicsScene( parent )
, doc_(Poppler::Document::load(DialogConfigure::cfg()->value("PDF_Files", "rules").toString()))
{
	setBackgroundBrush(QBrush(Qt::gray ,Qt::SolidPattern));
	int pos_y = 0;

	for (int id = 0; id < doc()->numPages(); id++) {
		// TODO need use real phisical device DPI
		QPixmap pixmap = QPixmap::fromImage(doc()->page(id)->renderToImage(100, 100));
		QGraphicsItem *item = addPixmap(pixmap);
		item->setPos( 20, pos_y );
		pos_y += (pixmap.height() + 20);
	}
}

Rules::~Rules()
{
	self_ = NULL;
}

Poppler::Document* Rules::doc()
{
	return doc_;
}

#include "rules.moc"
