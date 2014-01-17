#include <QApplication>
#include "kgen.h"

int main (int argc, char **argv)
{
	QApplication app(argc, argv);
	KGen *ui = new KGen();
	ui->show();
	return app.exec();
}