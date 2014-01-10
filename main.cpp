#include <QtGui/QApplication>
#include "cwallmanager.h"


int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    CWallManager foo;
    foo.show();
    return app.exec();
}
