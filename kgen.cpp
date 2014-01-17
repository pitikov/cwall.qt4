#include <QtGui>
#include <QtCore>

#include "kgen.h"

KGen::KGen()
: QWidget()
, ui(new Ui::KGen)
{
	ui->setupUi(this);
	ui->lineEditSerial->setReadOnly(true);
}

KGen::~KGen()
{

}

void KGen::changeData(const QString& arg)
{
	QString ctrlsumm;
	
	for (int i = 0; i<25; i++) {
		ctrlsumm += QCryptographicHash::hash(QString(ui->lineEditOrganization->text()+ui->lineEditEmail->text()+ui->lineEditName->text()).toUtf8(), QCryptographicHash::Md5).toHex().toUpper().left(25)[i];
		if ((i==4)|(i==9)|(i==14)|(i==19)) ctrlsumm+="-";
	}
	ui->lineEditSerial->setText(ctrlsumm);
}


#include "kgen.moc"