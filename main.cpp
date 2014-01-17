#include <QtGui/QApplication>
#include <QSplashScreen>

#include "rules.h"
#include "cwallmanager.h"


int main(int argc, char** argv)
{
    QApplication app(argc, argv);
		/* TODO change splash screen image
		 * 
		 * Need use qt-resource file and theme-correct splash-screen image */
		QImage img("/usr/share/wallpapers/Vector_Sunset/contents/screenshot.png");
		QSplashScreen splash(QPixmap::fromImage(img));
		splash.show();
		splash.showMessage(QApplication::tr("Get autenfication"));
    CWallManager foo;
		splash.showMessage(QApplication::tr("Prepare data"));
		Rules::sample();
    foo.showMaximized();
		splash.close();
    return app.exec();
}
