#ifndef RULES_H
#define RULES_H

#include <poppler/qt4/poppler-qt4.h>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QPixmap>
#include <QImage>
#include <QList>
#include <QQueue>

class Rules : public QGraphicsScene
{
    Q_OBJECT

public:
	static Rules* sample( QObject* parent = NULL );
	~Rules();
	Poppler::Document *doc();
	QString searchText() const;
	QList<QGraphicsRectItem*> search(const QString &str);
	//QGraphicsRectItem* searchNext(const QString &str);
	//QGraphicsRectItem* searchPrevious(const QString &str);
	void searchClean();
	QString searched;

private:
	static Rules *self_;
	Poppler::Document *doc_;
	QQueue<QGraphicsRectItem*> artefacts;

	Rules( QObject *parent );
	
};

#endif // RULES_H
