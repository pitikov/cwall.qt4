#include <QtGui/QApplication>
#include "cwall.h"


int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    cwall foo;
    foo.show();
    return app.exec();
}
