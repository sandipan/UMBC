#include <QtGui/QApplication>
#include "miningwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MiningWindow w;
    w.show();

    return a.exec();
}
