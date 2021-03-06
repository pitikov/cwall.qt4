#include <QtGui/QApplication>
#include <QSplashScreen>

#include "rules.h"
#include "cwallmanager.h"


int main(int argc, char** argv)
{
    QApplication app(argc, argv);
		QImage img("/usr/share/cwall/img/splash.jpg");
		QSplashScreen splash(QPixmap::fromImage(img));
		//splash.showMessage("<h3><font color=#E20071>"+QApplication::tr("get auntefication")+"</font></h3>");
		splash.show();
    CWallManager *managerWin = new CWallManager;
		//splash.showMessage("<h3><font color=#E20071>"+QApplication::tr("load data")+"</font></h3>");
		//splash.finish(managerWin);
		Rules::sample();
		managerWin->show();
		splash.close();
    return app.exec();
}
