#include <QtGui/QApplication>
#include "mipswindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MIPSWindow w;
    w.show();
    return a.exec();
}
