#include <QtGui/QApplication>
#include "ddmwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DDMWindow w;
    w.show();

    return a.exec();
}
