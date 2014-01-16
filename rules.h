#ifndef RULES_H
#define RULES_H

#include <poppler/qt4/poppler-qt4.h>
#include <QGraphicsScene>
#include <QPixmap>
#include <QImage>


class Rules : public QGraphicsScene
{
    Q_OBJECT

public:
	static Rules* sample( QObject* parent = NULL );
	~Rules();
	Poppler::Document *doc();

private:
	static Rules *self_;
	Poppler::Document *doc_;

	Rules( QObject *parent );

};

#endif // RULES_H
