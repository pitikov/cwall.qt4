#include <QtGui>
#include <QtCore>

#ifndef KGEN_CLASS
#define KGEN_CLASS
#include "ui_kgen.h"

namespace Ui {
	class KGen;
}

class KGen : public QWidget
{
	Q_OBJECT
	
public:
	explicit KGen();
	virtual ~KGen();
	
private slots:
	void changeData(const QString &arg);
private:
	Ui::KGen *ui; 
};
#endif // KGEN_CLASS